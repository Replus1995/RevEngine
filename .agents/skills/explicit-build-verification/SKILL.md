---
name: explicit-build-verification
description: Control build verification for RevEngine development tasks. Apply whenever editing, fixing, implementing, refactoring, reviewing, or following a plan in this repository; do not configure, generate, compile, link, or otherwise build for verification unless the user explicitly requests build verification or explicitly asks to execute an agreed plan that includes it.
---

# Explicit Build Verification

Treat build verification as opt-in for this repository.

- Do not run project generation, CMake configure/generate, compilation, linking, or equivalent build-validation commands merely because code was changed or a task was completed.
- Build only when the user explicitly asks for build verification, or explicitly asks to execute an agreed plan whose steps include build verification.
- Do not infer build authorization from requests such as “implement,” “fix,” “finish,” “check,” or “verify” unless the user clearly refers to a build.
- Static inspection and other non-build checks remain allowed when they are useful and within scope.
- When no build was authorized, finish by stating that build verification was not run because it is opt-in for this project.
