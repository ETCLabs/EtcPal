## Making a release of EtcPal

- [ ] Make sure there is a clean/working build on the `develop` branch.
- [ ] Review and update `CHANGELOG.md`
- [ ] Check `docs/mainpage.md` and `README.md` to make sure they are still accurate
- [ ] Check `ThirdPartySoftware.txt` to make sure it is still accurate and doesn't need any
      additions
- [ ] Generate docs for the version
  * Follow `tools/ci/publish_docs.sh` manually, except do `python generate_doxygen.py -r [3-digit release number]`
  * Add the version number to `versions.txt` on the `gh-pages` branch and mark it as latest
  * Prune any unused stage/ directories on the `gh-pages` branch.
  * Commit the updated `etcpal.tag` file on the current branch (`develop` or `release/v*`)
- [ ] Merge changes to `stable`
  * `git checkout stable`
  * `git merge --no-ff [develop|release/v*]`
- [ ] Create build and tag with `tools/version/create_build.py`
  * Include `-r` flag to specify a release build
- [ ] Mark as release on the releases page
- [ ] Create build report
- [ ] Merge `stable` back into `develop`

## Starting work on a new release

- [ ] Update 3-digit versions to next planned version in all relevant places:
  * CMakeLists.txt
  * Doxyfile
- [ ] Pick a new color for Doxygen
