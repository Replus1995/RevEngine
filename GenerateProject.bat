@echo ====== Generate Visual Studio Solution ======
@cmake -G "Visual Studio 17" -Ax64 -Bbuild
@if not "%1" == "" goto DONE
@if not exist build\.vs\RevEngine\v17\Browse.VC.opendb start build\RevEngine.sln

:DONE
@popd