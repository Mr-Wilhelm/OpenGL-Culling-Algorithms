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

<h2>References</h2>
<p>
Arrow operator - widely used in the script - https://www.geeksforgeeks.org/arrow-operator-in-c-c-with-examples/ <br>
Maths for matrix transformations - https://learnopengl.com/Getting-started/Transformations <br>
Maths for matrix transformations - https://catlikecoding.com/unity/tutorials/rendering/part-1/ <br>
LearnOpenGL shader class, code here is used in Shader.h - https://learnopengl.com/Getting-started/Shaders <br>
LearnOpenGL mesh class, code here is used in Mesh.h - https://learnopengl.com/Model-Loading/Mesh <br>
LearnOpenGL model class, code here is used in Model.h - https://learnopengl.com/Model-Loading/Model <br>
LearnOpenGL frustum culling, code here is used in CamView.h - https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling <br>
LearnOpenGL camera class, code here used in Camera.h - https://learnopengl.com/Getting-started/Camera <br>    
</p>
