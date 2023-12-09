# What's Holmgard?

Holmgard is a 3D game engine focused on huge worlds (think solar system sized) and procedural planets. The game engine has been "extracted" from the OSPGL project, as it grew to the point where it could be used as a general purpoe game engine.

The engine is not invasive on how you implement game logic, which is mostly written in Lua, and allows very easy extension using C++ plugins for performance critical code. Nonetheless, LuaJIT is used, which makes it perform excellently.

You can find a project which showcases most features of the engine here: https://github.com/tatjam/holmgard-demo

# How to use

This repository is meant to be included as a git submodule, 
your main repo containing the res/ and plugin/ folders where
the bulk of the game (lua / C++) is implemented. For a working use case, check the holmgard-demo repository.

# Core resources

## Option 1 - use embedded
You must define HOLMGARD_CORERES_DIR appropriately, so that the game engine can find the core resources. Typically, this will be whatever folder you use for the git submodule. 

## Option 2 - Use resource package
