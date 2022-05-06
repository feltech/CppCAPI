# C++ ⟷ C API

## What

A header-only template-based C++17 <-> C <-> C++17 plugin system.

Check out the `tests/demos` directory for example usage.

## Why

C++ plugin systems typically use `dlopen` et al. to load DSO module libraries during execution. This
requires the plugin and host application to use ABI-compatible compilers and libraries, complicating
plugin development and leading to hard-to-detect bugs. Further, there is limited support for binding
plugins developed in other languages to C++.

Compiler isolation through a pure C wall is a common technique to decouple C++ libraries, since C
has a long-standing stable ABI. A huge bonus of this technique is that a C API opens up the
possibility of bindings to a multitude of languages other than C++.

The main problem with this approach is the large amount of boilerplate code required to maintain C
bindings along with the C++ types. Another difficulty comes in the form of memory management bugs,
e.g. use-after-free, deallocation in the wrong DSO (if a different/custom allocator was used).

This project aims to provide a set of utilities and wrappers to reduce boilerplate and simplify
memory management, using modern C++ techniques.

## Summary

In terms of nomenclature, a given _host_ loads a _plugin_, and both host and plugin can act as a
_service_ and/or a _client_.

Objects are referenced across the pure C wall by opaque handles (pointers to incomplete types).
Member functions are implemented as function suites (`struct`s of function pointers). The signature
of suite functions must follow a specific pattern for C++ template lookups to work. Exceptions are
supported by converting to/from an error code and a `char[]` array (of some fixed size).

Templated helper classes are provided to detail mappings of opaque handles to types, and to
auto-convert handles to instances (original object for the service, wrapper/adapter objects for the
client).

## Memory management

The memory of an object is always owned (allocated/deallocated) by the service. The lifetime of
objects is controlled by, in order of performance/danger, `OwnedByService`, `OwnedByClient` or
`Shared` tags. Internally these map to raw pointers to an existing instance, pointers to a
(heap-allocated) new instance, and `std::shared_ptr`s, respectively. The `Shared` ownership model
ensures the eventual deallocation always occurs in the service - the client just sees an opaque
handle (heap-allocated instance of a `shared_ptr`), so is unaware of the distinction between
`OwnedByClient` and `Shared` handles.

## To do

In no particular order
* Cross-platform (currently Linux-only).
* Documentation and more examples.
* Better error messages.
* Monolithic header option.
