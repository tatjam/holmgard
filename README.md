This repository is meant to be included as a git submodule, 
your main repo containing the res/ and plugin/ folders where
the bulk of the game (lua / C++) is implemented.

# Core resources

## Option 1 - use embedded
You must define HOLMGARD_CORERES_DIR appropriately, so that the game engine can find the core resources. Typically, this will be whatever folder you use for the git submodule. 

## Option 2 - Use resource package
