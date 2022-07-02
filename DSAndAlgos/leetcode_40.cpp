// @problem : https://leetcode.com/problems/combination-sum-ii/

class Solution {
public:
    void dfs(const vector<int>& candidates, vector<vector<int>> &ans, vector<int> temp, int curr_idx, int target){
        // base case:
        if(target == 0) {
            ans.push_back(temp);
            return;
        }
        
        if(target < 0 || curr_idx >= candidates.size()) return;
        
        for(int i=curr_idx; i<candidates.size(); i++){
            // smart way to avoid duplicates
            if(i != curr_idx && candidates[i] == candidates[i-1]) continue;
            temp.push_back(candidates[i]);
            dfs(candidates,ans,temp,i+1,target-candidates[i]);
            temp.pop_back();
            
        }
        return; 
    }
    
    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        sort(candidates.begin(), candidates.end());
        vector<vector<int>> ans;
        vector<int> temp;
        dfs(candidates,ans, temp, 0, target);
        return ans;
    }
};
