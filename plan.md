1. **Optimize distance check in `simuv2/collide.cpp` (SimCarCollideResponse)**
   - Replace `float distpab = sgLengthVec2(pab); ... sgScaleVec2(tmpv, n, MIN(distpab, 0.05));`
   - Use `sgLengthSquaredVec2` and check against 0.05 squared (0.0025). But actually, `tmpv` uses the `distpab` value linearly... Wait, no, we can't completely avoid `sgLengthVec2` if we need the actual linear distance to scale `tmpv`. But let's look at `SimCarCollideAddDeformation` in `simuv3/collide.cpp`:
     `if (sgLengthVec3(collision_state->force) < sgLengthVec3(force))`
     We can definitely replace this with `sgLengthSquaredVec3(collision_state->force) < sgLengthSquaredVec3(force)`.

2. **Optimize distance check in `simuv3/collide.cpp` (SimCarCollideAddDeformation)**
   - Replace `if (sgLengthVec3(collision_state->force) < sgLengthVec3(force))` with `if (sgLengthSquaredVec3(collision_state->force) < sgLengthSquaredVec3(force))`.
   - This avoids computing two square roots for every deformation calculation, which happens often during collisions.

3. **Optimize aero damage in `simuv3/aero.cpp` (SimAeroDamage)**
   - Replace `if (sgLengthVec3(car->aero.rot_front) > 1.0)` with `if (sgLengthSquaredVec3(car->aero.rot_front) > 1.0)`
   - Do the same for `rot_lateral` and `rot_vertical`.
   - 1.0 squared is 1.0, so the threshold remains the same.
   - This avoids up to three `sqrt` calls per damage iteration.

4. **Complete pre-commit steps to ensure proper testing, verification, review, and reflection are done.**
   - Run verification checks.

5. **Submit the change.**
   - Submit the PR with title "⚡ Bolt: Avoid sqrt in collision and aero hot loops"
