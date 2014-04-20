The animation is the powerful Atlas, ascending to his throne and holding the Earth. He begins in front of his podium, facing away. With one powerful leap, he backflips 5 times and lands gracefully atop the pedestal just in time to catch the immense Earth. Around his tower are 12 smaller pillars comprised of small globes bouncing in periodic motion.

Atlas is built in a hierarchal fashion and adjusting his height will actual maintain all ratios of his build. Similarly, the bouncing pillars are hierarchal and can be treated as one object. 

Unfortunately, I was unable to get the polygon component working correctly.

Like the examples, I utilized the texture-mapped globes. I loved the idea and thought they were beautiful and fun to play with. I also played with the coloring of Atlas’ clothes for a while before deciding on the classed red shirt / blue pants combo.

POINT BREAKDOWN:

[4/4 points] At least one two-level hierarchical object (e.g., a human arm).
	I had multiple hierarchal objects: man and pillar.
[4/4 points] At least one texture, either mapped or procedural.
	Globes were texture mapped.
[4/4 points] 360 degree camera fly-around using LookAt.
	360 degree fly-around during flip
[0/6 points] At least one polygonal object modeled vertex by vertex (i.e., you must specify the vertex positions and normals directly), which must be shaded using the Phong local illumination model and flat shading (i.e., you must code a set of shaders that perform the required polygon rendering).
	I was not able to get the polygon function implemented correctly.
[2/2 points] Real-time speed. Make sure that your animation runs in real time on fast machines. “Real time” means that one virtual second corresponds roughly to one real second.
	Runs smooth and efficient.
[2/2 points] Display the frame rate of your program in the console window.
[2/2 points] Make and submit a movie of your animation using the provided mpeg_encode utility. The movie frame size should be 500 x 500 pixels. Include a 100 x 100 pixel representative image from your animation.
	I was not able to get the utility working properly. However, I still submitted a video.

[4/4 points] Creativity (story, colors, etc).
	It’s a story of Atlas! He leaps from a normal level to a place upon a pedestal, holding the weight of the world on his shoulders.
[4/4 points] Complexity.
	Multiple uses of trigonometric functions, periodic motion, phase shifts, and geometric principles. Slight simulation of gravity, and the bouncing globes actually compress and expand rather than just bouncing statically.

[5/5 points] Overall quality: Object and camera motion, scene construction, proper texturing, attention to detail.
	Camera motion is dynamic, introducing the scene starting from Atlas then the pillar. The entire scene is laid out during his flip, then focuses on him. As the world falls, everything comes in to view again.