import cv2
import numpy as np

# Real-world dimensions (mm)
image_height_mm = 4900  # height in mm
image_width_mm = 2400   # width in mm

# List to store points (with type)
points = []
is_weight = False  # Default mode is to select "weights"

# Load the corrected image
corrected_image_path = 'corrected_image.jpg'  # Use the previously corrected image
corrected_image = cv2.imread(corrected_image_path)
image_height, image_width, _ = corrected_image.shape

# Mouse callback function to capture clicks and display scaled coordinates
def click_event(event, x, y, flags, param):
    global is_weight
    if event == cv2.EVENT_LBUTTONDOWN:
        # Flip the coordinates for origin at the upper-right corner
        flipped_x = image_width - x  # x-axis flipped
        flipped_y = y  # y-axis not flipped
        
        # Scale the clicked coordinates to real-world dimensions
        real_x_mm = (flipped_x / image_width) * image_width_mm
        real_y_mm = (flipped_y / image_height) * image_height_mm
        
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

# Create a window and set the mouse callback
cv2.namedWindow("Corrected Image")
cv2.setMouseCallback("Corrected Image", click_event)

# Overlay 6 horizontal lines
line_color = (0, 255, 0)  # Green color for lines
line_thickness = 1
for i in range(1, 8):  # Drawing 6 lines
    y = int(i * image_height / 8)  # Evenly spaced along the height
    cv2.line(corrected_image, (0, y), (image_width, y), line_color, line_thickness)

# Overlay two central circles
circle_color = (0, 255, 255)  # Yellow color for circles
circle_radius = 20  # Radius of the circles in pixels
circle_thickness = 2  # Thickness of the circle border
center_x = image_width // 2
center_y = image_height // 2

# Draw the first central circle
cv2.circle(corrected_image, (center_x, center_y-400), circle_radius, circle_color, circle_thickness)

# Draw the second circle just below the first one
cv2.circle(corrected_image, (center_x, center_y + 200), circle_radius, circle_color, circle_thickness)

# Instruction for user
print("Click anywhere on the corrected image to select points.")
print("Press 'w' to toggle between selecting 'weights' and 'targets'.")
print("Press 'q' to finish and display the points in reverse order.")

# Display the image with lines and circles, and wait for user input
cv2.imshow("Corrected Image", corrected_image)

while True:
    key = cv2.waitKey(1) & 0xFF
    if key == ord('w'):
        toggle_mode()
    elif key == ord('q'):
        break

# Close the window
cv2.destroyAllWindows()

# Print the points in reverse order
print("\nPoints in reverse order (with format):")
for point in reversed(points):
    x, y, is_weight_point = point
    print(f"addPoint((map_point_t){{{x:.2f}, {y:.2f}, {is_weight_point}}});")
print(f"num_targets = {len(points)};")
