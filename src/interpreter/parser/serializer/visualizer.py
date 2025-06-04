import json
import argparse
from graphviz import Digraph
import uuid

class ASTVisualizer:
    def __init__(self, json_data):
        self.ast = json_data
        self.graph = Digraph('AST', format='png')
        self.graph.attr(rankdir='TB', size='10,10', ratio='fill')
        self.node_count = 0
        
        # Color scheme for different node types
        self.colors = {
            "compound": "#E0E0E0",
            "Binary Op": "#FFD700",
            "Unary Op": "#FFA07A",
            "Var": "#98FB98",
            "Num Literal": "#87CEFA",
            "String Literal": "#DDA0DD",
            "Bool Literal": "#40E0D0",
            "Nil Literal": "#D3D3D3",
            "If Block": "#FF6347",
            "For Block": "#9370DB",
            "While Block": "#7B68EE",
            "break statement": "#DC143C",
            "continue statement": "#FF4500",
            "function": "#4682B4",
            "function call": "#6495ED",
            "return statement": "#20B2AA",
            "Subscript": "#F08080",
            "List": "#90EE90",
            "BAD": "#FF0000"
        }
        
        # Shape scheme
        self.shapes = {
            "compound": "folder",
            "Binary Op": "diamond",
            "Unary Op": "diamond",
            "Var": "ellipse",
            "Num Literal": "box",
            "String Literal": "box",
            "Bool Literal": "box",
            "Nil Literal": "box",
            "If Block": "hexagon",
            "For Block": "hexagon",
            "While Block": "hexagon",
            "break statement": "triangle",
            "continue statement": "triangle",
            "function": "component",
            "function call": "cds",
            "return statement": "invtriangle",
            "Subscript": "house",
            "List": "note",
            "BAD": "octagon"
        }
        
        # Keep track of processed nodes to avoid multiple edges
        self.processed_edges = set()
    
    def generate_node_id(self):
        """Generate a unique node ID"""
        self.node_count += 1
        return f"node_{self.node_count}"
    
    def visualize_node(self, node, parent_id=None, edge_label=None):
        """Recursively visualize nodes in the AST"""
        if not isinstance(node, dict) or "type" not in node:
            return None
        
        node_id = self.generate_node_id()
        node_type = node["type"]
        
        # Create label based on node type
        if node_type == "Binary Op":
            op_symbol = node.get("operator", "unknown")
            label = f"{node_type}\nOperator: {op_symbol}"
        elif node_type == "Unary Op":
            op_symbol = node.get("operator", "unknown")
            label = f"{node_type}\nOperator: {op_symbol}"
        elif node_type == "Var":
            label = f"{node_type}\nID: {node['id']}"
        elif node_type == "Num Literal":
            label = f"{node_type}\nValue: {node['value']}"
        elif node_type == "String Literal":
            # Truncate long strings
            val = node["value"]
            if len(val) > 20:
                val = val[:17] + "..."
            label = f"{node_type}\nValue: \"{val}\""
        elif node_type == "Bool Literal":
            label = f"{node_type}\nValue: {node['value']}"
        elif node_type == "function":
            label = f"{node_type}"
        else:
            label = node_type
        
        # Add node to graph
        shape = self.shapes.get(node_type, "ellipse")
        color = self.colors.get(node_type, "#FFFFFF")
        self.graph.node(node_id, label, style='filled', fillcolor=color, shape=shape)
        
        # Connect to parent if exists
        if parent_id and edge_label:
            edge_key = (parent_id, node_id, edge_label)
            if edge_key not in self.processed_edges:
                self.graph.edge(parent_id, node_id, label=edge_label)
                self.processed_edges.add(edge_key)
        
        # Process children based on node type
        if node_type == "compound":
            if "children" in node and isinstance(node["children"], list):
                for child in node["children"]:
                    self.visualize_node(child, node_id, "child")
        
        elif node_type == "Binary Op":
            if "left" in node:
                self.visualize_node(node["left"], node_id, "left")
                    
            if "right" in node:
                self.visualize_node(node["right"], node_id, "right")
        
        elif node_type == "Unary Op":
            if "operand" in node:
                self.visualize_node(node["operand"], node_id, "operand")
        
        elif node_type == "If Block":
            if "condition" in node:
                self.visualize_node(node["condition"], node_id, "condition")
            
            if "body" in node:
                self.visualize_node(node["body"], node_id, "body")
        
        elif node_type in ["For Block", "While Block"]:
            if "condition" in node:
                self.visualize_node(node["condition"], node_id, "condition")
            
            if "iterator" in node:
                self.visualize_node(node["iterator"], node_id, "iterator")
            
            if "range" in node:
                self.visualize_node(node["range"], node_id, "range")
            
            if "body" in node:
                self.visualize_node(node["body"], node_id, "body")
        
        elif node_type == "function":
            # Create a single parameters container node
            if "params" in node and isinstance(node["params"], list) and node["params"]:
                params_id = self.generate_node_id()
                self.graph.node(params_id, "Parameters", style='filled', fillcolor="#E0FFFF", shape="tab")
                self.graph.edge(node_id, params_id, label="params")
                
                for i, param in enumerate(node["params"]):
                    param_id = self.visualize_node(param, params_id, f"param {i+1}")
            
            if "body" in node:
                self.visualize_node(node["body"], node_id, "body")
        
        elif node_type == "function call":
            if "func" in node:
                self.visualize_node(node["func"], node_id, "function")
            
            # Create a single arguments container node
            if "params" in node and isinstance(node["params"], list) and node["params"]:
                params_id = self.generate_node_id()
                self.graph.node(params_id, "Arguments", style='filled', fillcolor="#E0FFFF", shape="tab")
                self.graph.edge(node_id, params_id, label="args")
                
                for i, param in enumerate(node["params"]):
                    param_id = self.visualize_node(param, params_id, f"arg {i+1}")
        
        elif node_type == "return statement":
            if "expr" in node:
                self.visualize_node(node["expr"], node_id, "expression")
        
        elif node_type == "List":
            if "data" in node and isinstance(node["data"], list):
                for i, item in enumerate(node["data"]):
                    self.visualize_node(item, node_id, f"item {i+1}")
        
        elif node_type == "Subscript":
            if "var expr" in node:
                self.visualize_node(node["var expr"], node_id, "variable")
            
            if "left" in node:
                self.visualize_node(node["left"], node_id, "index/start")
            
            if "right" in node and node.get("is_slice", False):
                self.visualize_node(node["right"], node_id, "end")
        
        return node_id
    
    def render(self, output_path="ast_visualization"):
        """Render the graph to a file"""
        self.visualize_node(self.ast)
        self.graph.render(output_path, view=True, cleanup=True)
        return f"{output_path}.png"

def main():
    parser = argparse.ArgumentParser(description='Visualize AST from JSON')
    parser.add_argument('json_file', help='Path to the JSON file containing the AST')
    parser.add_argument('-o', '--output', default='ast_visualization', help='Output file path (without extension)')
    args = parser.parse_args()
    
    try:
        with open(args.json_file, 'r') as f:
            # Filter out any lines that are not valid JSON
            valid_json_lines = []
            for line in f:
                if line.strip() and not line.startswith('now try to dispatch'):
                    valid_json_lines.append(line)
            
            json_content = ''.join(valid_json_lines)
            ast_data = json.loads(json_content)
            
            visualizer = ASTVisualizer(ast_data)
            output_file = visualizer.render(args.output)
            print(f"Visualization saved to {output_file}")
    
    except json.JSONDecodeError as e:
        print(f"JSON parsing error: {e}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()