#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <functional>
#include <cmath>
#include <iostream>
#include <cassert>

class Value;

using ValuePtr = std::shared_ptr<Value>;

struct Hash {
    size_t operator()(const ValuePtr value) const;
};


class Value : public std::enable_shared_from_this<Value>
{
public:
    inline static size_t currentID = 0;
    float data;
    float grad;
    std::string op; // "+"
    size_t id;
    std::vector<ValuePtr> prev;
    std::function<void()> backward;

private:
    Value(float data, const std::string &op, size_t id) : data(data), op(op), id(id) {};

public:
    static ValuePtr create(float data, const std::string &op = "")
    {
        return std::shared_ptr<Value>(new Value(data, op, Value::currentID++));
    }
    
    ~Value(){
        --Value::currentID;
    }
    
    static ValuePtr add(const ValuePtr& lhs, const ValuePtr& rhs){
        auto out = Value::create(lhs->data+rhs->data, "+");
        out->prev = {lhs, rhs};
        out->backward = [lhs_weak = std::weak_ptr<Value>(lhs), rhs_weak = std::weak_ptr<Value>(rhs), out_weak = std::weak_ptr<Value>(out)](){
            lhs_weak.lock()->grad +=  out_weak.lock()->grad;
            rhs_weak.lock()->grad +=  out_weak.lock()->grad;
        };
        return out;
    }
    
    static ValuePtr multiply(const ValuePtr& lhs, const ValuePtr& rhs){
        auto out = Value::create(lhs->data*rhs->data, "*");
        out->prev = {lhs, rhs};
        out->backward = [lhs_weak = std::weak_ptr<Value>(lhs), rhs_weak = std::weak_ptr<Value>(rhs), out_weak = std::weak_ptr<Value>(out)](){
            lhs_weak.lock()->grad +=  rhs_weak.lock()->data * out_weak.lock()->grad;
            rhs_weak.lock()->grad +=  lhs_weak.lock()->data * out_weak.lock()->grad;
        };
        return out;
    }
    
    static ValuePtr subtract(const ValuePtr& lhs, const ValuePtr& rhs){
        auto out = Value::create(lhs->data-rhs->data, "-");
        out->prev = {lhs, rhs};
        out->backward = [lhs_weak = std::weak_ptr<Value>(lhs), rhs_weak = std::weak_ptr<Value>(rhs), out_weak = std::weak_ptr<Value>(out)](){
            lhs_weak.lock()->grad +=  out_weak.lock()->grad;
            rhs_weak.lock()->grad -=  out_weak.lock()->grad;
        };
        return out;
    }

    // out = base^exponent
    // dL/d(base) = dL/d(out) *    d(out)/d(base)
    //            = out->grad *    exponent * base^(exponent-1)

    static ValuePtr pow(const ValuePtr& base, float exponent) {
        float newValue = std::pow(base->data, exponent);
        auto out = Value::create(newValue, "^");
        out->prev = {base};
        out->backward = [base_weak = std::weak_ptr<Value>(base), out_weak = std::weak_ptr<Value>(out), exponent](){
            if (auto base = base_weak.lock()) {
                base->grad += exponent * std::pow(base->data, exponent - 1) * out_weak.lock()->grad;
            }
        };
        return out;
    }
    // c = a/b
    // c = a*b^-1
    static ValuePtr divide(const ValuePtr& lhs, const ValuePtr& rhs) {
        auto reciprocal = pow(rhs, -1);
        return multiply(lhs, reciprocal);
    }

    static ValuePtr relu(const ValuePtr& input) {
        float val = std::max(0.0f, input->data);
        auto out = Value::create(val, "ReLU");
        out->prev = {input};
        out->backward = [input, out]() {
            // 0 * out_grad
            // local_grad * out_grad
            if (input) input->grad += (out->data > 0) * out->grad;
        };
        return out;
    }


    static ValuePtr sigmoid(const ValuePtr& input) {
        float x = input->data;
        float t = std::exp(x) / (1 + std::exp(x));
        auto out = Value::create(t, "Sigmoid");
        out->prev = {input};
        out->backward = [input, out]() {
            input->grad += t * (1 - t) * out->grad;
        });
        return out;
    }

    
    void buildTopo(std::shared_ptr<Value> v, std::unordered_set<std::shared_ptr<Value>, Hash>& visited, std::vector<std::shared_ptr<Value>>& topo){
         if(visited.find(v) == visited.end()){
             visited.insert(v);
             for(const auto& child : v->prev){
                 buildTopo(child, visited, topo);
             }
             topo.push_back(v);
         }
     }

    
    void backProp(){
        std::vector<std::shared_ptr<Value>> topo;
        std::unordered_set<std::shared_ptr<Value>, Hash> visited;
        buildTopo(shared_from_this(), visited, topo); // this will create a graph for us
        
        this->grad = 1.0f;
        
        for( auto it = topo.rbegin(); it != topo.rend(); ++it ){
            if((*it)->backward)  {
                (*it)->backward();
            }
            (*it)->print();
        }
    }
    
    void print(){
        std::cout << "[data=" << this->data << ", grad=" << this->grad << "]\n";
    }
    
 
};

size_t Hash::operator()(const ValuePtr value) const {
    return std::hash<std::string>()(value.get()->op) ^ std::hash<float>()(value.get()->data);
}


// Part 4

enum ActivationType{
    RELU,
    SIGMOID
};

class Activation{
    static std::shared_ptr<Value> Relu(const std::shared_ptr<Value>& val){
        return Value::relu(val);
    }
    static std::shared_ptr<Value> Sigmoid(const std::shared_ptr<Value>& val){
        return Value::sigmoid(val);
    }

public:
    static inline std::unordered_map<ActivationType, std::function<std::shared_ptr<Value>(std::shared_ptr<Value>&)>> mActivationFcn = {
            {ActivationType::RELU, Relu},
            {ActivationType::SIGMOID, Sigmoid}
    }
};













// Function to generate a random float between -1 and 1
float getRandomFloat() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(-1, 1);
    return dis(gen);
}

class Neuron {
private:
    std::vector<ValuePtr> weights;
    ValuePtr bias = Value::create(0.0);
    const ActivationType activation_t;

public:
    Neuron(size_t nin, const ActivationType& activation_t) : activation_t(activation_t) {
        for (size_t idx = 0; idx < nin; ++idx) {
            weights.emplace_back(Value::create(getRandomFloat()));
        }
    }

//    // For testing
//    Neuron(size_t nin, float val, const ActivationType& activation_t = ActivationType::SIGMOID)
//            : activation_t(activation_t) {
//        for (size_t idx = 0; idx < nin; ++idx) {
//            weights.emplace_back(Value::create(getRandomFloat()));
//        }
//    }

    void zeroGrad() {
        for (auto& weight : weights) {
            weight->grad = 0;
        }
        bias->grad = 0;
    }

    // Dot product of a Neuron's weights with the input
    ValuePtr operator()(const std::vector<ValuePtr>& x) {
        if (x.size() != weights.size()) {
            throw std::invalid_argument("Vectors must be of the same length");
        }

        ValuePtr sum = Value::create(0.0);

        for (size_t idx = 0; idx < weights.size(); ++idx) {

            ValuePtr intermediateVal = Value::multiply(x[idx], weights[idx]);
            sum = Value::add(sum, intermediateVal);
        }

        // Add bias
        //sum->add_inplace(bias);
        sum = Value::add(sum, bias);

        // Apply activation function
        const auto& activationFcn = Activation::mActivationFcn.at(activation_t);
        return activationFcn(sum);
    }

    std::vector<ValuePtr> parameters() const {
        std::vector<ValuePtr> out;
        out.reserve(weights.size() + 1);

        out.insert(out.end(), weights.begin(), weights.end());
        out.push_back(bias);

        return out;
    }

    void printParameters() const {
        printf("Number of Parameters: %zu\n", weights.size() + 1);
        for (const auto& param : weights) {
            printf("%f, %f\n", param->data, param->grad);
        }
        printf("%f, %f\n", bias->data, bias->grad);
        printf("\n");
    }

    size_t getParametersSize() const {
        return weights.size() + 1;
    }
};




















int main()
{
    auto a = Value::create(1.0, "");
    auto b = Value::create(2.0, "");
    //Value b;
    auto c = Value::add(a, b); // 3
    
    auto d = Value::multiply(c,c); // 9
    
    assert(c->data == 3.0);
    assert(c->op == "+");
    
    assert(d->data == 9.0);
    assert(d->op == "*");
    
    auto loss = Value::add(d, d); // 9+9 = 18
    
    loss->backProp();
    
    
    
    // auto d = subtract(a-b);

    // auto e = multiply(c*d);

    // a -->
    //        +    -->   c ->           L   ->  [a,b,c,d,L]
    
    // b -->             +
                        
                        // d
    
    //auto a = std::shared_ptr<Value>();
}
