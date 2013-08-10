import cv2
import numpy as np

image = cv2.imread( 'test.jpg' )

samples = image.reshape( ( -1 , 3 ) )
samples = np.float32( samples )

K = 8
criteria = ( cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER , 10 , 1.0 )
attempts = 10
flags = cv2.KMEANS_RANDOM_CENTERS

retval, bestLabels, centers = cv2.kmeans( samples , K , None , criteria , attempts , flags )

centers = np.uint8( centers )
result = centers[ bestLabels.flatten() ]
results = result.reshape( ( image.shape ) )

cv2.imshow( 'K-Means Clustering' , results )

cv2.waitKey( 0 )
cv2.destroyAllWindows()
