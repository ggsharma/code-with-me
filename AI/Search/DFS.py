from collections import deque


class Node:
    nodeNumber = 0

    def __init__(self):
        Node.nodeNumber = Node.nodeNumber + 1
        self.uuid = Node.nodeNumber
        print("Building: ", self.uuid)
        self.children = []

    def construct_and_get_child_node(self, child: object) -> object:
        """

        :param child:
        :return:
        """
        self.children.append(child)
        # print("Attaching child: ", self.uuid)
        return child

    def __eq__(self, other):
        return self.uuid == other.uuid


class DFS:
    def __init__(self):
        self.root = None
        self.curr = None
        self.branching_factor = None
        self.depth = None


    @staticmethod
    def build_child_nodes_breadth_first(root, branching_factor, depth):
        if depth <= 0:
            return

        queue = deque([(root, 0)])

        while queue:
            current_node, current_depth = queue.popleft()

            if current_depth >= depth:
                return

            for idx in range(1, branching_factor + 1):
                child_node = current_node.construct_and_get_child_node(Node())
                queue.append((child_node, current_depth + 1))


    def search(self, target_node):
        self.curr = self.root
        path = []
        return self.find_target(self.curr, target_node, path, 0)


    def find_target(self, curr_node, target_node, path, depth):
        if curr_node.uuid == target_node:
            return True, path

        elif depth >= self.depth:
            return False, []

        target_found = (False, [])

        path.append(curr_node.uuid)

        for child in curr_node.children: # 1,2,3
            target_found = target_found[0] or self.find_target(child, target_node, path, depth + 1)
            if target_found[0]:
                return target_found

        path.pop()

        return False, path

    def pretty_print_graph(self,node, indent=0, parent_arrow=""):
        print("  " * indent + f"{parent_arrow}Node: {node.uuid}")
        for child in node.children:
            self.pretty_print_graph(child, indent + 1, "│")

    def print_graph(self):
        self.pretty_print_graph(self.root)

    def build_graph(self, branching_factor=3, depth=3):
        self.root = Node()
        self.branching_factor = branching_factor
        self.depth = depth
        return DFS.build_child_nodes_breadth_first(self.root, branching_factor, depth)


dfs = DFS()

dfs.build_graph()
print("Total nodes: ", Node.nodeNumber)

# # dfs.print_graph()
#
path = dfs.search(39)
print(path)

