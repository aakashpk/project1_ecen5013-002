Here's a simple example of vscode debugger configuration.
This can be modified to use the top level makefile instead.
Suggesting symlinking to this misc/..../.vscode location rather than tracking the .vscode directory directly, so we don't clobber each other's configs.
Example symlink command:
ln -s misc/miles_config/.vscode .vscode

