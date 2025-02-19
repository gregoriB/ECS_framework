# TODOs

## Performance
- Memory pools
- Update certain vectors to be spans in:
  * ComponentWrapper class
    * Modified vector
    * .unpack return value
  * Grouping class
    * Entity Ids
    * Stored component sets
  * EntityComponentManager class
    * .getEntityIds return value
- Rethink component transformation pipeline implementation

## Features
- Shared components
- Custom tags for use with batching components

## Tasks
- Rename and refactor grouping approach
- Remove Effect component after custom tags are implemented
- Remove Timer class after Effect component is removed
  * Update benchmark tests to use different timer
- Refactor test organization
- Implement a more robust testing framework?
- Add better benchmarks
  * Compare against other ECS libraries
- Private constructors for internal classes
