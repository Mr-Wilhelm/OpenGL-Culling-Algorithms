A code editor/IDE is required to run this program, there is no external .exe file
Load the .sln file in the code compiler (preferably visual studio)
And compile and run the code.

How to Use:

All actions should only be done in Main.cpp.
In the int main() function, there is a section which looks like:

    //----------SELECT ENVIRONMENT HERE----------
    //------CHOICES: DENSE, SPARSE, DYNAMIC------

    //chosenEnvironment = DENSE;
    chosenEnvironment = SPARSE;
    //chosenEnvironment = DYNAMIC;
    //chosenEnvironment = DEFAULT;

    //-------------------------------------------

Here you can change the environment before runtime by commenting out specific lines. Only one environment can be chosen, otherwise 

During Runtime:
- WASD keys to move
- 1 - enable back face culling
- 2 - disable back face culling
- 3 - enable frustum culling
- 4 - disable frustum culling
- 5 - enable Z Culling
- 6 - disable Z Culling
