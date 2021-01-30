def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    nodes = PriorityQueue()
    fringe = []
    path = []
    explored=set([])
    priority=0
    dict={}
    start_node = problem.getStartState()
    if problem.isGoalState(start_node):
        return 'already in goal'
    else:
        nodes.push((start_node, path),priority)
        dict[start_node] = 0
        explored.add(start_node)
        while (nodes):
            curr, path = nodes.pop()

            if problem.isGoalState(curr):
                return path
            else:
                next = problem.getSuccessors(curr)
                for node in nodes.heap:
                    fringe.append(node[0])
                for states in next:
                        if states[0] not in (key for key in dict):
                            cost=problem.getCostOfActions(path + [states[1]])
                            nodes.push((states[0], path + [states[1]]),cost)
                            dict[states[0]]=cost
                            explored.add(states[0])
                        elif states[0] in (key for key in dict) and (problem.getCostOfActions(path + [states[1]]) < dict[states[0]]) :
                            cost = problem.getCostOfActions(path + [states[1]])
                            nodes.push((states[0], path + [states[1]]), cost)
                            dict[states[0]] = cost
                            explored.add(states[0])
