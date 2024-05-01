# Pie Engine
## Description

## Prerequisites
1. Download SDK for your system from https://vulkan.lunarg.com/sdk/home or use default package manager on Linux Systems (check build instructions)
2. Clone repo by
   ```
   git clone git@github.com:PikemenStudio/pie-engine.git && cd pie-engine
   ```
3. run
   ```
   git submodule update --init --recursive
   ```
4. Setup vcpkg
   ```
   ./vcpkg/bootstrap-vcpkg.sh -disableMetrics
   ```
## Build Instructions
### Apple
Run
```
bash ./scripts/macos_install.sh
```

**If you will use other terminal or app to build project, export variables globally (check for them at the end of the script)**

Build project and fix errors by downloading missing packages using brew if any
```
mkdir build || cd build && cmake ../ && make
```
### Windows
Not tested yet
### Linux
Use default package manager to download SDK
```
wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.280-jammy.list https://packages.lunarg.com/vulkan/1.3.280/lunarg-vulkan-1.3.280-jammy.list
sudo apt update
sudo apt install vulkan-sdk
```
and after fixing several errors (after next step you'll get 'package not found' error) run
```
mkdir build || cd build && cmake ../ && make
```
Instructions how to add other package version you can find on Vulkan SDK page

Otherwize create an issue
## Contributing

### Basic process
1. Create new branch from main with any name
2. Work in it :)
3. Create pull request to merge updates to main with SQUASH COMMITS option and the name of the commit according rule: "[brief] description" (like "[sound] fix sound; implement spaceship")
4. Check for all actions (format, tidy, tests)
5. Wait for merging
6. Sleep well

### Modules overview
Every module is a folder with two subfolders: `facade` and `src`.
- `facade`: headers (and ONLY HEADERS) which are needed for the user of this module. No implementations of any functions should be here, only declarations. This is needed to have the ability to change the implementation as much as possible without changing the facade.
- `src`: implementations of the functions (functions in broad sense) declared in `facade`. `facade` SHOULD NOT depend on `src` (don't include any headers from `src` in `facade`), `src` should depend on `facade`.

### Facade's architecture

1. All facade structs: any struct that is used in the facade (params, return values, etc.)
Can be moved to separate file
```c++
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// maybe should be namespace
struct SomeFacadeStructs {
  // any structs or enums
  
  // Also they can be templates, if they depend on implementation
  // e.g. they hold module reference (dependency)
};
```
2. Concept for template, that defines template type for module implementation
```c++
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <typename T, typename Dependency/*, here may be more dependencies*/>
concept SomeModuleImpl = requires(T Obj) {
  // Must be constructible with that type
  std::is_constructible_v<T, SomeFacadeStructs::SomeProps<Dependency/*, all dependencies*/> &&>;
  // Must have method void test(int)
  {
    Obj.test(int)
  } -> std::same_as<void>;
};
```
3. Blueprint (macro) for facade implementation, MUST be consistent with the concept
```c++
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the SomeApi Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SOME_API_IMPL(name)                                                     \
  template <Dependency DependencyImpl> class GraphicApiFacade##name##Impl {     \
  public:                                                                       \
    SomeApiFacade##name##Impl(const SomeApiFacade##name##Impl &&) =             \
        delete;                                                                 \
    SomeApiFacade##name##Impl(SomeFacadeStructs::SomeProps<DependencyImpl> &&); \
    ~SomeApiFacade##name##Impl();                                               \
                                                                                \
    void test(int a);                                                           \
  protected:                                                                    \
    void *Data; // Any type, only implementation from .cpp knows how to work with it
```
4. Module implementation declaration
```c++
namespace some_api_impls {
SOME_API_IMPL(Some)
} // namespace some_api_impls
```
5. Facade class
```c++
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for the GraphicApi
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <Dependency DepencencyImpl> class SomeApiFacade {
public:
  using Impl = some_api_impls::SomeApiFacadeVulkanImpl<DepencencyImpl>;

  SomeApiFacade(SomeFacadeStructs::SomeEngineProps<DepencencyImpl> &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  [[maybe_unused]] Impl ImplInstance;
};
```
Use `ImplInstance` to work with all facade methods, facade is just a fabric and additional facade, that give opportunity to work with several implementations (switch them) easily
6. In src folder of module you can find .cpp (or should create it), that implements (define) all methods from blueprint

> If you want to work with different dependencies in different way (e.g. library require native type from depencency library) you can use template specialization

> Note: You MUST add explicit instantiation of the template class in the **end** of the .cpp file
> ```c++
> template class some_api_impls::SomeApiFacadeSomeImpl<
>    depencency_api_impls::DepencencyApiFacadeSomeDepencencyImpl>;
> ```

> Usage: Do you want to use module in the module of yours? Include facade header, add dependencies of that module as templates to your module class (blueprint) (and to concept) and provide that types to facade. That's all, you will get one big module with a lot of dependencies in templates and user will have opportunity to control them in one place

## Usage
