﻿# CS-C3100 Computer Graphics, Fall 2019
# Lehtinen / Aarnio, Kemppinen, Rehn
#
# Assignment 4: Physical Simulation

Student name: Saku Myllymaki
Student number: 715298
Hours spent on requirements (approx.): 10
Hours spent on extra credit (approx.): 4 

# First, a 10-second poll about this assignment period:

Did you go to exercise sessions?
Yes
Did you work on the assignment using Aalto computers, your own computers, or both?
Both
# Which parts of the assignment did you complete? Mark them 'done'.
# You can also mark non-completed parts as 'attempted' if you spent a fair amount of
# effort on them. If you do, explain the work you did in the problems/bugs section
# and leave your 'attempt' code in place (commented out if necessary) so we can see it.

     R1 Euler integrator (1p): done
        R2 Spring system (2p): done
 R3 Trapezoid integrator (2p): done
      R4 Pendulum system (2p): done
         R5 Cloth system (3p): done

# Did you do any extra credit work?
RK4 integrator. Rising smoke particle system for which there is an extra button in the GUI. The plane blocking the particles can be toggled by pressing P. Using a low step size makes the smoke look better. 
Random wind force for cloth system, wind can be toggled by pressing W. I also made the wind direction interpolate between two random directions so that the
directions are updated once every 10000 steps. To make the interpolation smoother, I used a trigonometric interpolator ( can I have extra points for this? :) ).
Both adding particles in the fluid system and changing the wind direction are implemented with a step() method.
(Describe what you did and, if there was a substantial amount of work involved, how you did it. Also describe how to use/activate your extra features, if they are interactive.)

# Are there any known problems/bugs remaining in your code?
No
(Please provide a list of the problems. If possible, describe what you think the cause is, how you have attempted to diagnose or fix the problem, and how you would attempt to diagnose or fix it if you had more time or motivation. This is important: we are more likely to assign partial credit if you help us understand what's going on.)

# Did you collaborate with anyone in the class?
No
(Did you help others? Did others help you? Let us know who you talked to, and what sort of help you gave or received.)

# Any other comments you'd like to share about the assignment or the course so far?

(Was the assignment too long? Too hard? Fun or boring? Did you learn something, or was it a total waste of time? Can we do something differently to help you learn? Please be brutally honest; we won't take it personally.)

