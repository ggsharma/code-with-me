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
