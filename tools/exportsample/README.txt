This tool exports the Win2D samples such that they can be built outside of the
Win2D repo.

The "exportsample.cmd" script can drive this process.  Normal usage looks like
this:

C:\Path\To\Win2D> exportsample c:\Path\To\Samples


It can also be configured so that the samples attempt to restore nuget packages
from a specified directory:

C:\Path\To\Win2D> exportsample c:\Path\To\Samples /PackageSource:\\networkshare\path\to\nuget\directory
