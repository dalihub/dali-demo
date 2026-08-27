This test stresses property update

  - It creates actors (deliberately with no renderer and therefore costs nothing to
    draw), which are fully live scene-graph nodes that receive property messages.
  - Each actor registers multiple custom animatable properties with uniform mappings.
  - Every tick, every actor updates multiple custom properties via SetProperty().
  - There is a burst mode with actor churn. Periodic bursts of extra actors are
    spawned and destroyed to keep the update volume varying throughout the run.
