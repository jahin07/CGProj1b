# CGProj1b
Purpose

Interact with smooth curves in OpenGL.
cccc
Set Up

Use the reorganized code base and fill in the code you wrote for Project 1a to enable picking. 
Place N=8N=8 control points equally distributed to form a circle. 
The title of the window to your name and your UFID, e.g. "Firstname Lastname (ufid)"

Task 1: Connect the pointsff

 
Points: 5
Connect all the control points with white lines

In order to draw a closed polygon and also draw the line in white instead of blue(the points' color), you may want to create another Vertex array of size ncpoints + 1. Copy the coordinate from cpoints, set the color to white and make sure the curve is closed.
Set up the VAO, VBO for this new array and call glDrawArrays with GL_LINE_STRIP to draw the polygon.
Task 2: Subdivision

 
Points: 10
From the polygon in task #1, generate a smoother curve by subdivision.

Initialize P0i=PiPi0=Pi (green). 
Double the number of control points by setting

Pk2i:=Pk−1i−2+10Pk−1i−1+5Pk−1i16
P2ik:=Pi−2k−1+10Pi−1k−1+5Pik−116
Pk2i+1:=5Pk−1i−1+10Pk−1i+Pk−1i+116
P2i+1k:=5Pi−1k−1+10Pik−1+Pi+1k−116
where kk is the level of subdivison and 
ii is the index of points is in range 0…(N×2k−1)0…(N×2k−1) .

When k=0k=0 show the original control polygon. When key 1 is pressed and k=mk=m then compute and draw the control polygon at level k=m+1k=m+1. However, when m=5m=5, mm is set to 0 and the original control polygon is drawn.

Task 3: C3C3 Bezier

Points: 10
Another way to generate a curve is to construct several smoothly connected Bezier pieces.

We are going to construct 8 pieces of degree 4. The iith piece has control points ci={ci,0,ci,1,ci,2,ci,3,ci,4}ci={ci,0,ci,1,ci,2,ci,3,ci,4}. One of the 5 control points is given as:
ci,2=4Pi−1+16Pi+4Pi+124
ci,2=4Pi−1+16Pi+4Pi+124
and the points ci,1ci,1 and ci3ci3 have the following form:
ci,1=aPi−1+bPi+cPi+1
ci,1=aPi−1+bPi+cPi+1
ci,3=cPi−1+bPi+aPi+1
ci,3=cPi−1+bPi+aPi+1
for constants a,b,ca,b,c.

Determine ci,0ci,0, ci,1ci,1, ci,3ci,3 and ci,4ci,4 so that the iith and the (i+1)(i+1)th Bezier piece join C3C3 . Write down the formulas for the control points into a readme.txt file.
Task 4: Draw Bezier curve

 
Points: 15
For each Bezier curve piece:

Choose nn, the tessellation number, i.e. the number of pieces to discretize each Bezier piece into, sufficiently high so the discretized curve looks smooth.

For each Bezier curve piece, evaluate the points on the curve when t=0,1n,2n,…,1t=0,1n,2n,…,1 using De Casteljau's algorithm and connect the evaluation points with a polyline.
When 2 is pressed, show/hide the Bezier curves.
