<h4 align="center">A Hex Editor for the Casio FX-CP400 with HollyHock-3</h4>

<p align="center">
  <a href="#how-to-use">How To Use</a> •
  <a href="#download">Download</a> •
  <a href="#credits">Credits</a> •
  <a href="#community">Community</a>
</p>

<p align="center">
  <img src="https://raw.githubusercontent.com/PyCSharp/HexEditor-For-HHK3/refs/heads/main/image.png" alt="CPHexEditor Preview">
</p>

---

## How To Use

1. Copy the `.hh3` file into the **root directory** of your calculator  
   (do not place it inside a subfolder).

2. Launch the calculator and open the Hex Editor through Hollyhock Launcher.  

3. Controls:  
   - Cursor: `←` / `→`  
   - A, B, C, D, E, F: `=` / `𝒙` / `𝒚` / `𝒛` / `^` / `÷`  
   - `Clear` = quit  

4. **Dumping memory:**  
   - Navigate to the **start address** (use the arrows top right).  
   - Press `EXP` → sets **start**.  
   - Navigate to the **end address**.  
   - Press `EXP` again → sets **end**.  
   - Press `EXE` → dumps memory from *start* to *end*.  

   **Notes:**  
   - Do **not select very large ranges**, as dumping too much memory at once can take a long time or fail.  
   - During the dump process, the calculator will **freeze temporarily** — this is normal.  
   - Once the dump is complete, a file named **`dump.dmp`** will appear in the calculator’s root directory.  

5. **Searching memory:**  
   - Enter your search pattern in hex (e.g. `>48656C6C6F` for `"Hello"`).  
   - Press `(-)` (negative key) to start searching.  
   - Results are shown **directly on the screen** *and* written to **`FoundAddresses.txt`** in the root directory.  

   **Range of search:**  
   - If you **set start & end** with `EXP` → only that range is searched.  
   - If you **do not set any range** → the **entire memory area** is searched.

6. **Read/Write Control Registers**
   - Click the `More Options` Button
   - Click on `Read/Write Control Registers`
   - Navigate using the `↓`/`↑` Buttons
   - Press `EXE` an type the value you want
   - Press `EXE` to apply the changes

---

## Download

[Download the latest release here](https://github.com/PyCSharp/HexEditor-For-HHK3/releases)

The release contains the `.hh3` file you just need to copy onto your calculator.

---

## Credits

- [Original Code for HHK2](https://github.com/SnailMath/CPhexEditor) by **SnailMath**
- [HHK3 Template](https://github.com/QBos07/HHK3template) by **QBos07**

---

## Community

Join the discussion, share feedback and get help on the Snail Math Discord Server:  
[https://discord.com/invite/knpcNJTzpd](https://discord.com/invite/knpcNJTzpd)  
