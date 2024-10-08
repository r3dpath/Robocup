import cv2
import numpy as np

# Real-world dimensions (mm)
image_height_mm = 4900  # height in mm
image_width_mm = 2900   # width in mm

# List to store points (with type for weights/targets)
points = []

# List to store navigation mesh nodes and connections
nodes = []
connections = []

# Mode control
is_weight = True  # Default mode is to select "weights"
placing_nodes = False  # Default mode is NOT to place nodes
selecting_connections = False  # Mode to connect nodes

# Load the corrected image
corrected_image_path = 'corrected_image.jpg'  # Use the previously corrected image
corrected_image = cv2.imread(corrected_image_path)
image_height, image_width, _ = corrected_image.shape

# Mouse callback function to capture clicks and display scaled coordinates
def click_event(event, x, y, flags, param):
    global is_weight, placing_nodes, selecting_connections, selected_node_1

    if event == cv2.EVENT_LBUTTONDOWN:
        # Flip the coordinates for origin at the upper-right corner
        flipped_x = image_width - x  # x-axis flipped
        flipped_y = y  # y-axis not flipped
        
        # Scale the clicked coordinates to real-world dimensions
        real_x_mm = (flipped_x / image_width) * image_width_mm
        real_y_mm = (flipped_y / image_height) * image_height_mm
        
        if placing_nodes:
            # Add the node to the nodes list
            nodes.append((real_x_mm, real_y_mm))
            node_id = len(nodes)  # ID is the position in the list
            print(f"Placed node {node_id} at ({real_x_mm:.2f} mm, {real_y_mm:.2f} mm)")
            
            # Draw node on image
            cv2.circle(corrected_image, (x, y), 7, (0, 255, 0), -1)
            cv2.putText(corrected_image, f"N{node_id}", (x + 10, y), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
            cv2.imshow("Corrected Image", corrected_image)
        
        elif selecting_connections:
            # Find the nearest node
            nearest_node_id = None
            nearest_distance = float('inf')
            for i, (nx, ny) in enumerate(nodes):
                # Calculate Euclidean distance
                node_x_img = image_width - (nx / image_width_mm) * image_width
                node_y_img = (ny / image_height_mm) * image_height
                dist = np.sqrt((x - node_x_img)**2 + (y - node_y_img)**2)
                if dist < nearest_distance:
                    nearest_distance = dist
                    nearest_node_id = i
            
            if nearest_node_id is not None:
                if 'selected_node_1' not in globals():
                    # Select the first node
                    selected_node_1 = nearest_node_id
                    print(f"Selected node {selected_node_1 + 1} for connection start.")
                else:
                    # Select the second node and create a connection
                    selected_node_2 = nearest_node_id
                    if selected_node_1 != selected_node_2:
                        connections.append((selected_node_1, selected_node_2))
                        print(f"Connected node {selected_node_1 + 1} and node {selected_node_2 + 1}")
                        
                        # Draw connection on the image
                        node1_x_img = image_width - (nodes[selected_node_1][0] / image_width_mm) * image_width
                        node1_y_img = (nodes[selected_node_1][1] / image_height_mm) * image_height
                        node2_x_img = image_width - (nodes[selected_node_2][0] / image_width_mm) * image_width
                        node2_y_img = (nodes[selected_node_2][1] / image_height_mm) * image_height
                        
                        cv2.line(corrected_image, (int(node1_x_img), int(node1_y_img)),
                                 (int(node2_x_img), int(node2_y_img)), (0, 255, 255), 2)
                        cv2.imshow("Corrected Image", corrected_image)
                    del selected_node_1  # Reset after creating the connection
        else:
            # Store the point along with its type (1 for weight, 0 for target)
            points.append((real_x_mm, real_y_mm, int(is_weight)))
            
            # Choose color: Blue for weights, Red for targets
            color = (255, 0, 0) if is_weight else (0, 0, 255)
            
            # Display the point on the image
            cv2.circle(corrected_image, (x, y), 5, color, -1)
            label = "W" if is_weight else "T"
            cv2.putText(corrected_image, f"{label}({real_x_mm:.2f}mm, {real_y_mm:.2f}mm)", (x, y-10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1)
            cv2.imshow("Corrected Image", corrected_image)

# Function to toggle between weight and target modes
def toggle_mode():
    global is_weight
    is_weight = not is_weight
    mode = "Weights" if is_weight else "Targets"
    print(f"Switched to selecting {mode}")

# Function to toggle placing nodes mode
def toggle_placing_nodes():
    global placing_nodes
    placing_nodes = not placing_nodes
    mode = "Placing Nodes" if placing_nodes else "Normal Mode"
    print(f"Switched to {mode}")

# Function to toggle connecting nodes mode
def toggle_selecting_connections():
    global selecting_connections
    selecting_connections = not selecting_connections
    mode = "Connecting Nodes" if selecting_connections else "Normal Mode"
    print(f"Switched to {mode}")

# Create a window and set the mouse callback
cv2.namedWindow("Corrected Image")
cv2.setMouseCallback("Corrected Image", click_event)

# Instruction for user
print("Click anywhere on the corrected image to select points.")
print("Press 'w' to toggle between selecting 'weights' and 'targets'.")
print("Press 'n' to toggle node placement mode.")
print("Press 'c' to toggle connecting nodes mode.")
print("Press 'q' to finish.")

# Display the image and wait for user input
cv2.imshow("Corrected Image", corrected_image)

while True:
    key = cv2.waitKey(1) & 0xFF
    if key == ord('w'):
        toggle_mode()
    elif key == ord('n'):
        toggle_placing_nodes()
    elif key == ord('c'):
        toggle_selecting_connections()
    elif key == ord('q'):
        break

# Close the window
cv2.destroyAllWindows()

# Print the points in reverse order
print("\nPoints in reverse order (with format):")
for point in reversed(points):
    x, y, is_weight_point = point
    print(f"addPoint((map_point_t){{{x:.2f}, {y:.2f}, {is_weight_point}}});")
