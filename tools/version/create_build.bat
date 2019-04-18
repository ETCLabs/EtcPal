@echo off
setlocal

cmake -DNEW_VERSION_NUMBER=%1 -P update_version_files.cmake 

IF %ERRORLEVEL% NEQ 0 GOTO END

SET /P CONTINUE=Check the updated version files, then press 'y' to commit and tag. Press any other key to abort. 
IF /I "%CONTINUE%" == "Y" (
  git commit -F tmp_commit_msg.txt
  git tag -a -F tmp_tag_msg.txt v%1
) ELSE (
  git reset HEAD ../..
)

:END
endlocal
