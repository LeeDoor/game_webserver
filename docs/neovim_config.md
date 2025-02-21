# Neovim Configuration file
I am using [Neovim](neovim.io) to edit my source code. I have a special configuration file in the root directory called `init.lua`. to use it, install neovim and **Packer**:
```bash
git clone --depth 1 https://github.com/wbthomason/packer.nvim \
~/.local/share/nvim/site/pack/packer/start/packer.nvim
```
Then you need to move `init.lua` to the `~/.config/nvim` directory. When you are done, launch neovim and start PackerSync:
```bash
nvim
# inside neovim:
:PackerSync
```

# Sync with CMake
If you want to change C++ files with neovim, you need to build the main project and create link to `compile_commands.json` from your root directory:
```bash
ln -s build/compile_commands.json .
```
