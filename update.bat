robocopy \\grfxaltbld1\canvas\docs . /mir /np /nc /ns /nfl /ndl /njh /njs /xf update.bat /xd .git
git add --all .
git commit -m "Update docs"
