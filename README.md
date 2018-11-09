# Pitchfork

Pitchfork is a set of conventions for native C and C++ projects. The most
prominent being [the project layout conventions](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs).

The layout specification document is available in `data/spec.bs`.


## Why the Name *Pitchfork*?

The very first public unveiling, drafting, and discussion of these project
conventions started with [a Reddit thread entitled "Prepare thy Pitchforks"](https://www.reddit.com/r/cpp/comments/996q8o/prepare_thy_pitchforks_a_de_facto_standard/). Until
that point, I had not chosen any particular name for the conventions, but I
felt "Pitchfork" was as apt a name as any.


## The `pf` Tool

This repository also hosts a (currently experimental) tool that helps you
create and work with Pitchfork-compliant projects.

This project is still very young and has a while to go before being a useful
developer tool. Once ready, this README will be updated with proper user
documentation.


## The `pf` Library

The `pf` tool mentioned above is built upon the `pf` library, also hosted in
this repository. This library can be used to query and manipulate
Pitchfork-compliant projects.
