# CGProj1b
Purpose

Interact with smooth curves in OpenGL.

Set Up

Place N=8 control points equally distributed to form a circle. 
The title of the window to your name and your UFID, e.g. "Firstname Lastname (ufid)"

Task 1: Connect the points

 
Connect all the control points with white lines

In order to draw a closed polygon and also draw the line in white instead of blue(the points' color), you may want to create another Vertex array of size ncpoints + 1. 
Copy the coordinate from cpoints, set the color to white and make sure the curve is closed.
Set up the VAO, VBO for this new array and call glDrawArrays with GL_LINE_STRIP to draw the polygon.

Task 2: Subdivision

 
From the polygon in task #1, generate a smoother curve by subdivision.

Initialize P0i=Pi (green). 
Double the number of control points by setting

Pk2i:=(Pk−1i−2+10Pk−1i−1+5Pk−1i)/16
Pk2i+1:=(5Pk−1i−1+10Pk−1i+Pk−1i+1)/16

where k is the level of subdivison and 
i is the index of points is in range 0…(N×2k−1).

When k=0 show the original control polygon. When key 1 is pressed and k=m then compute and draw the control polygon at level k=m+1. However, when m=5, m is set to 0 and the original control polygon is drawn.

Task 3: C3 Bezier

Another way to generate a curve is to construct several smoothly connected Bezier pieces.

We are going to construct 8 pieces of degree 4. The ith piece has control points ci={ci,0,ci,1,ci,2,ci,3,ci,4}. One of the 5 control points is given as:
ci,2=(4Pi−1+16Pi+4Pi+1)/24
and the points ci,1ci,1 and ci3ci3 have the following form:
ci,1=aPi−1+bPi+cPi+1
ci,3=cPi−1+bPi+aPi+1
for constants a,b,c.

Determine ci,0, ci,1, ci,3 and ci,4 so that the ith and the (i+1)th Bezier piece join C3 . Write down the formulas for the control points into a readme.txt file.

Task 4: Draw Bezier curve

 
For each Bezier curve piece:

Choose n, the tessellation number, i.e. the number of pieces to discretize each Bezier piece into, sufficiently high so the discretized curve looks smooth.

For each Bezier curve piece, evaluate the points on the curve when t=0,1/n,2/n,…,1 using De Casteljau's algorithm and connect the evaluation points with a polyline.
When 2 is pressed, show/hide the Bezier curves.
