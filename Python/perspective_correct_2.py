import cv2
import numpy as np

# Variables to store the corners of the trapezoid
points = []
selected_point = None
dragging = False

# Function to draw the trapezoid and evenly spaced lines corrected for perspective
def draw_trapezoid(image, points):
    if len(points) == 4:
        # Draw the trapezoid
        for i in range(4):
            cv2.line(image, tuple(points[i]), tuple(points[(i+1) % 4]), (0, 255, 0), 2)

        # Compute perspective-corrected lines
        src_points = np.float32([points[0], points[1], points[2], points[3]])
        width = int(np.linalg.norm(np.array(points[0]) - np.array(points[1])))
        height = int(np.linalg.norm(np.array(points[0]) - np.array(points[3])))

        dst_points = np.float32([[0, 0], [width, 0], [width, height], [0, height]])

        # Perspective transformation matrix
        matrix = cv2.getPerspectiveTransform(src_points, dst_points)

        # Calculate evenly spaced points in the transformed space
        step = height // 8  # 7 lines -> 8 spaces
        for i in range(1, 8):
            y = i * step

            # Calculate the corresponding points in the original trapezoid space
            top_line = np.array([0, y, 1])
            bottom_line = np.array([width, y, 1])

            # Use the inverse perspective transformation to map back
            inv_matrix = np.linalg.inv(matrix)

            top_orig = inv_matrix @ top_line
            bottom_orig = inv_matrix @ bottom_line

            top_orig = top_orig[:2] / top_orig[2]
            bottom_orig = bottom_orig[:2] / bottom_orig[2]

            # Draw the perspective-corrected horizontal lines
            cv2.line(image, tuple(top_orig.astype(int)), tuple(bottom_orig.astype(int)), (255, 0, 0), 1)

# Mouse callback function for dragging points
def mouse_event(event, x, y, flags, param):
    global points, selected_point, dragging, image, original_image

    if event == cv2.EVENT_LBUTTONDOWN:
        # Check if we are clicking near any point
        for i, point in enumerate(points):
            if abs(x - point[0]) < 10 and abs(y - point[1]) < 10:
                selected_point = i
                dragging = True
                break

    elif event == cv2.EVENT_MOUSEMOVE:
        if dragging and selected_point is not None:
            points[selected_point] = [x, y]  # Update the position of the selected point
            temp_image = original_image.copy()  # Reset to original each time
            draw_trapezoid(temp_image, points)
            cv2.imshow("Select Trapezoid", temp_image)

    elif event == cv2.EVENT_LBUTTONUP:
        dragging = False
        selected_point = None
        # Redraw the final trapezoid
        image = original_image.copy()  # Reset to original
        draw_trapezoid(image, points)
        cv2.imshow("Select Trapezoid", image)

# Load the image
image_path = 'IMG_1970.jpg'  # Replace with your image path
image = cv2.imread(image_path)
original_image = image.copy()

# Initialize the four corners of the trapezoid (start with a default shape)
height, width = image.shape[:2]
points = [
    [width // 4, height // 4],
    [3 * width // 4, height // 4],
    [3 * width // 4, 3 * height // 4],
    [width // 4, 3 * height // 4]
]

# Create a window and set the mouse callback
cv2.namedWindow("Select Trapezoid")
cv2.setMouseCallback("Select Trapezoid", mouse_event)

# Display the image with the initial trapezoid
draw_trapezoid(image, points)
cv2.imshow("Select Trapezoid", image)
cv2.waitKey(0)

# Ensure that four points have been finalized
if len(points) == 4:
    # Convert points to a NumPy array
    src_points = np.float32(points)

    # Define the dimensions of the output image (width and height of the transformed image)
    width = int(np.linalg.norm(np.array(points[0]) - np.array(points[1])))
    height = int(np.linalg.norm(np.array(points[0]) - np.array(points[3])))

    # Define the destination points for a top-down view
    dst_points = np.float32([[0, 0], [width, 0], [width, height], [0, height]])

    # Compute the perspective transform matrix
    matrix = cv2.getPerspectiveTransform(src_points, dst_points)

    # Apply the perspective transformation
    warped_image = cv2.warpPerspective(original_image, matrix, (width, height))

    # Display the corrected image
    cv2.imshow("Corrected Image", warped_image)
    cv2.imwrite('corrected_image.jpg', warped_image)
    cv2.waitKey(0)

cv2.destroyAllWindows()
