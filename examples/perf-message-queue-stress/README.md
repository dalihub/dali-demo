This test is to stresses the Update<->Event message queue.

It stresses both directions of the queue:

  - Event -> Update: every tick, every actor gets several SetProperty() calls
    (position/size/color), which is the dominant source of message queue
    traffic in real apps (animations, layouting, touch feedback, etc).
  - Update -> Event: every actor has a property notification watching a
    property that changes every tick, so notification manager is kept busy
    relaying notifications back to the Event thread every frame.
  - Actor lifecycle churn: a fraction of actors are destroyed and recreated
    periodically, to exercise the message-based ownership handover paths
    rather than just steady-state property writes.
  - Burst mode: it makes the Event->Update producer bursty instead of
    steady-state: most ticks send the normal message volume, but some
    ticks send multiple times as many.

