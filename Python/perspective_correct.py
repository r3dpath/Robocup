import cv2
import numpy as np

# Initialize a list to store the points
points = []

# Mouse callback function to capture clicks
def select_points(event, x, y, flags, param):
    global points
    if event == cv2.EVENT_LBUTTONDOWN:
        if len(points) < 4:
            points.append([x, y])
            cv2.circle(image, (x, y), 5, (0, 255, 0), -1)
            cv2.imshow("Select Corners", image)
        if len(points) == 4:
            #cv2.destroyWindow("Select Corners")
            pass

# Load the image
image_path = 'IMG_1950.jpg'  # Replace with your image path
image = cv2.imread(image_path)
original_image = image.copy()

# Create a window and set the mouse callback
cv2.namedWindow("Select Corners")
cv2.setMouseCallback("Select Corners", select_points)

# Display the image and wait for the user to select four corners
print("Click on the four corners of the object in clockwise order, starting from the top-left.")
cv2.imshow("Select Corners", image)
cv2.waitKey(0)

# Ensure that four points have been selected
if len(points) != 4:
    print("Error: Please select exactly four points.")
else:
    # Convert the points to a NumPy array
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
