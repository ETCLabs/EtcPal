## Making a release of lwpa

- [ ] Make sure there is a clean/working build on the `develop` branch.
- [ ] Review and update `CHANGELOG.md`
- [ ] Check `docs/mainpage.md` and `README.md` to make sure they are still accurate
- [ ] Generate docs for the version
  * Follow `tools/ci/publish_docs.sh` manually, except replace "head" with version number in the
    form "vM.m"
  * Add the version number to `versions.txt` on the `gh-pages` branch and mark it as latest
  * `PROJECT_NUMBER` in the Doxyfile should be correct but might need to be updated for patch
    versions
  * Commit the updated `lwpa.tag` file on the current branch (`develop` or `release/v*`)
- [ ] Merge changes to `master`
  * `git checkout master`
  * `git merge --no-ff [develop|release/v*]`
- [ ] Create build and tag with `tools/version/create_build.py`
  * Include `-r` flag to specify a release build
- [ ] Mark as release on the releases page
- [ ] Create build report
- [ ] Merge `master` back into `develop`

## Starting work on a new release

- [ ] Update 3-digit versions to next planned version in all relevant places:
  * CMakeLists.txt
  * Doxyfile
- [ ] Pick a new color for Doxygen
