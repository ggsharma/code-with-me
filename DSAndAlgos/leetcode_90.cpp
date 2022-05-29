// Coded by Gautam Sharma

// input = [1,2,2]
// bitset = 
// [0,0,0] ; [1,2,2] => []
// [1,0,0] ; [1,2,2] => [1]
// [0,1,0] ; [1,2,2] => [2]
// [0,0,1] ; [1,2,2] =>
// [1,1,1] ; [1,2,2] => [1,2,2]
// 
// ......  2^n ; n = 3; 8

class Solution {
public:
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        // input = [1,2,2]
        // bitset = [0,0,0], [1,0,0], [0,1,0]
        int super_set = pow(2,nums.size());
        vector<vector<int>> ans;
        unordered_set<string> seen;
        // [1,2,1,1]
        // [2,1] "21" [1,2] "12"
        // [1,2] "12" [1,2] "12"
        
        sort(nums.begin(), nums.end());
        // decimal = 1 -> [0,0,1]
        // decimal = 2 -> [0,1,0]
        // decimal = 3 -> [0,1,1]
        //                [1,0,0]
        for(int decimal=0; decimal < super_set; decimal++){
            vector<int> temp;
            string hashcode ="";
            for(int i=0; i< nums.size(); i++){
                // decimal = 2 -> [0,1,0]
                int mask = 1 << i;               
                if(decimal&mask){
                    temp.push_back(nums[i]);
                    hashcode += to_string(nums[i]);
                    // [1,1,1,2] 
                    // temp [1,2]
                    // hashcode "12"
                }
            }   
            
            if(seen.find(hashcode) == seen.end()){
                ans.push_back(temp);
                seen.insert(hashcode);
            }
                      
        }
              
        return ans;

        
    }
};
