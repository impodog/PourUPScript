# How to write a PUPS extension

## Dependencies

1. You need to clone this repository.
2. Use CMake to build project pups_Lib, and you should get a dynamic library.
3. You're ready to go!

## Creating my project

Here we use CMake project as example.

Start by adding **CMakeList.txt** under your project folder.

In the CMake file, you'll need to *include* the PUPS library, and *link* to the dynamic library you have just built.

Here's one possible version:

```cmake
# Configuring CMake and C++ (it's required though!)
cmake_minimum_required(VERSION 3.27)
set(CXX_STANDARD 17)

# Set the path to the PUPS repo
set(PUPS_PATH "./PourUPScript")

# Creating a new project "myProject"
project(myProject)

# Include PUPS directory
include_directories(${PUPS_PATH})

# Link to the directory with the library
# You may also change this directory to where you built the library
link_directories(${PUPS_PATH}/cmake-build-release)

# Building this project(DO change you source file names!)
add_library(myProject SHARED mySource.cpp)

# Finally, link to the PUPS library
target_link_libraries(myProject pups_Lib)
```

## Writing my project

PUPS extension are built as dynamic libraries. It is simple to write them. For example:

```c++
// Include necessary components
#include <cstdio>
#include <memory>

// Add the pups header to file
#include "pups.h"

// Implement a function
pups::ObjectPtr fast_print(pups::FunctionArgs &args, pups::Map *map) {
    while (!args.empty()) {
        printf("%s", args.front()->get()->str().c_str());
        args.pop_front();
    }
    putchar('\n');
    return pups::pending;
}

// Creating a name, or "Id" to my function
pups::Id id_fast_print{"", "fast_print"};

// Finally, write the init function of this library
// You must add extern "C" to link this function properly
extern "C" void PUPS_Init_myProject(pups::PackageHandler &handler) {
    handler.add(id_fast_print, std::make_shared<pups::Function>(fast_print));
    printf("My project is inited!\n");
}
```

Let me explain how this works.

### Functions

In the function, we wrote like this:

```c++
pups::ObjectPtr <function-name>(pups::FunctionArgs &args, pups::Map *map) { ... }
```

Actually, all PUPS functions come in this format, where `args` is the a `std::deque<pups::ObjectPtr>` holding all the arguments that the script gives.

Then, all scripts are executed in a map, so the map is also given.

Finally, it returns a pointer to some value.

But in this example, we did not return a useful value, instead, `pups::pending`, which stands for nothing!



### Id

In the example, we wrote a line to implement a `pups::Id`, which is the name for certain variables.

The constructor of `pups::Id` takes two variables, one for qualifiers(should NOT be used here), and one for the name.

So, we created an id with name "fast_print"



### Initialize

The initialization functions should come in form "PUPS_Init_\<library name>", and the name should stay the same with the built library name.

For example, your built library is "libtest.dll", so your init function name will be "PUPS_Init_test".



The function takes a reference to `pups::PackageHandler` as parameter. It supports *add* and *new_sub_const*, to see more, go to source/constants.h

Now, to add your function into the handler, just use this function:

`handler.add(<Id:name>, <ObjectPtr:object>)`

Also, the function itself isn't a PUPS object, but `pups::Function` is. So you should make a `std::shared_ptr` containing `pups::Function` as the second argument.

## Importing my project

After building the dynamic library, you may import it.

Here we introduce a Puppy built-in function "import_lib". It takes two arguments, one for the library directory, and one for the file name(**WITHOUT** "lib" and ".dll" or ".so").

Let's assume you have file "libmyProject.dll" under folder "myProjectFolder", then you can write code like this:

```puppyscript
import_lib "myProjectFolder" "myProject"
```

BUT, doing so is temporary and not reusable. So it is better to wrap it in a normal PUPS library, for example "myProject.pups", and then *import* it in other files.