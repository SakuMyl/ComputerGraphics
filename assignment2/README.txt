# CS-C3100 Computer Graphics, Fall 2019
# Lehtinen / Aarnio, Kemppinen, Rehn
#
# Assignment 2: Curves and Surfaces

Student name: Saku Myllymäki
Student number: 715298
Hours spent on requirements (approx.): 9
Hours spent on extra credit (approx.): 1

# First, some questions about how you worked on this assignment.
# Your answers in this section will be used to improve the course.
# They will not be judged or affect your points, but please answer them.

- Did you go to lectures?
Some
- Did you go to exercise sessions?
Yes
- Did you work on the assignment using Aalto computers, your own computers, or both?
Both, mostly Aalto computers
# Which parts of the assignment did you complete? Mark them 'done'.
# You can also mark non-completed parts as 'attempted' if you spent a fair amount of
# effort on them. If you do, explain the work you did in the problems/bugs section
# and leave your 'attempt' code in place (commented out if necessary) so we can see it.

                        R1 Evaluating Bezier curves (2p): done
                      R2 Evaluating B-spline curves (2p): done
       R3 Subdividing a mesh into smaller triangles (2p): done
        R4 Computing positions for the new vertices (2p): done
R5 Smoothing the mesh by repositioning old vertices (2p): done

# Did you do any extra credit work?
Boundary case handling for subdivision surfaces. I modified the weights for new edge vertices in case an edge had no neighbor.
For new positions of old vertices, I looped through the 1-ring both clockwise and counter-clockwise to find the boundary vertices
and used the weights described in http://graphics.stanford.edu/~mdfisher/subdivision.html.
(Describe what you did and, if there was a substantial amount of work involved, how you did it. Also describe how to use/activate your extra features, if they are interactive.)

# Are there any known problems/bugs remaining in your code?
No.
(Please provide a list of the problems. If possible, describe what you think the cause is, how you have attempted to diagnose or fix the problem, and how you would attempt to diagnose or fix it if you had more time or motivation. This is important: we are more likely to assign partial credit if you help us understand what's going on.)

# Did you collaborate with anyone in the class?
No
(Did you help others? Did others help you? Let us know who you talked to, and what sort of help you gave or received.)

# Any other comments you'd like to share about the assignment or the course so far?
The assignment pdf was a bit difficult to read. Some materials related to exercise R4 was inside the exercise description of R3 which was really confusing. Also it would've been
nice to mention that the icosahedron is supposed to look like a spikeball in the description of R3 or R4, not in the beginning of the description of R5. I spent some time figuring 
out if I did something wrong when in fact that was how it was supposed to look like. The subdivision visualisations and the index arrays (neighborTris, neighborEdges etc.) 
given in the pdf were really helpful, though. The exercises were challenging and (mostly) fun (if the long debugging sessions related to subdivision surfaces can be put aside).
(Was the assignment too long? Too hard? Fun or boring? Did you learn something, or was it a total waste of time? Can we do something differently to help you learn? Please be brutally honest; we won't take it personally.)

