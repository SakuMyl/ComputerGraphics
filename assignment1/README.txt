# CS-C3100 Computer Graphics, Fall 2019
# Lehtinen / Aarnio, Kemppinen, Rehn
#
# Assignment 1: Introduction

Student name: Saku Myllymäki
Student number:	715298
Hours spent on requirements (approx.):
Hours spent on extra credit (approx.):

# First, some questions about where you come from and how you got started.
# Your answers in this section will be used to improve the course.
# They will not be judged or affect your points, but please answer all of them.
# Keep it short; answering shouldn't take you more than 5-10 minutes.

- What are you studying here at Aalto? (Department, major, minor...?)
Computer Science
- Which year of your studies is this?
Second
- Is this a mandatory course for you?
No
- Have you had something to do with graphics before? Other studies, personal interests, work?
I made a simple 3D visualisation in Programming Studio 2 project 
- Do you remember basic linear algebra? Matrix and vector multiplication, cross product, that sort of thing?
Yes
- How is your overall programming experience? What language are you most comfortable with?
First year mandatory courses in Computer Science plus experience with JavaScript and TypeScript from summer job.
I would consider myself most comfortable with either Scala or JavaScript
- Do you have some experience with these things? (If not, do you have experience with something similar such as C or Direct3D?)
C++: Little
C++11: Little
OpenGL: No experience
Very basics of C (pointers, arrays, strings). 
- Have you used a version control system such as Git, Mercurial or Subversion? Which ones?
I have used Git during my whole first year in university and in work this summer.
- Did you go to the technology lecture?
Yes
- Did you go to exercise sessions?
Yes
- Did you work on the assignment using Aalto computers, your own computers, or both?
Both
# Which parts of the assignment did you complete? Mark them 'done'.
# You can also mark non-completed parts as 'attempted' if you spent a fair amount of
# effort on them. If you do, explain the work you did in the problems/bugs section
# and leave your 'attempt' code in place (commented out if necessary) so we can see it.

(Try to get everything done! Based on previous data, virtually everyone who put in the work and did well in the first two assignments ended up finishing the course, and also reported a high level of satisfaction at the end of the course.)

                            opened this file (0p): done
                         R1 Moving an object (1p): done
R2 Generating a simple cone mesh and normals (3p): done
  R3 Converting mesh data for OpenGL viewing (3p): done
           R4 Loading a large mesh from file (3p): done

# Did you do any extra credit work?
I did all the especially recommended extras and the medium difficulty alternative for the normal transforms.
The object can be scaled with keys W and S and rotated with A and D. To achieve this, I added a scale and a translation matrix which modify the modelToWorld matrix. 
I made my camera rotate around the object always pointing at it. The camera can be moved to all directions by moving a mouse while pressing down the left mouse button. I moved the camera's origin to the object's location and then applied the rotation and camera distance translation and added some event handlers to react to mouse movements and left mouse button presses. (I'm not sure whether this is a virtual trackball implementation or something else entirely). 
(Describe what you did and, if there was a substantial amount of work involved, how you did it. Also describe how to use/activate your extra features, if they are interactive.)

# Are there any known problems/bugs remaining in your code?
The simple triangle example model is lit both sides every half a turn and completely dark every other half a turn (when it is rotated). This could be fixed by having to triangles very closely next to each other and their normals pointing to opposite directions. This, however, I don't really care about. The object can be scaled to both directions unlimitedly, which makes it look ugly when it's very close to being scaled to 0. I could limit the scaling factor to some value when it approaches 0 but I don't think this is a big problem.   
(Please provide a list of the problems. If possible, describe what you think the cause is, how you have attempted to diagnose or fix the problem, and how you would attempt to diagnose or fix it if you had more time or motivation. This is important: we are more likely to assign partial credit if you help us understand what's going on.)

# Did you collaborate with anyone in the class?

(Did you help others? Did others help you? Let us know who you talked to, and what sort of help you gave or received.)
I explained how to calculate normals for the cone (userGeneratedModel) using cross product to someone in a practice session.
# Any other comments you'd like to share about the assignment or the course so far?

(Was the assignment too long? Too hard? Fun or boring? Did you learn something, or was it a total waste of time? Can we do something differently to help you learn? Please be brutally honest; we won't take it personally.)
The mandatory exerices were fun and not too challenging. Jumping into the OpenGL code in the extras and having to read the OpenGL documentation religiously wasn't something I enjoyed but I guess it can't be helped. 
