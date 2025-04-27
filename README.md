# Chip-8 Interpreter
My implementation of the Chip-8 instruction set to interpret games and programs designed for it.
Technical specifications were found at [Wikipedia](https://en.wikipedia.org/wiki/CHIP-8).

# Technologies
* [SDL3](https://github.com/libsdl-org/SDL)

# Building
Only requirement is to obtain a copy of SDL3 from their repository and place it in the `ext/` 
directory.

1. `mkdir build`
2. `cd build`
3. `cmake ../`
4. `make`
5. `cp "../ext/SDL3-3.2.10/x86_64-w64-mingw32/bin/SDL3.dll" "./SDL3.dll"`

# Usage
`./c8i program`

### Controls:
<table>
    <tr>
        <th> Original </th>
        <th> Mapped</th>
    </tr>
    <tr>
        <td>
            <table>
                <tr> 
                    <td> 1 </td>
                    <td> 2 </td>
                    <td> 3 </td>
                    <td> C </td>
                </tr>
                <tr> 
                    <td> 4 </td>
                    <td> 5 </td>
                    <td> 6 </td>
                    <td> D </td>
                </tr>
                <tr> 
                    <td> 7 </td>
                    <td> 8 </td>
                    <td> 9 </td>
                    <td> E </td>
                </tr>
                <tr> 
                    <td> A </td>
                    <td> 0 </td>
                    <td> B </td>
                    <td> F </td>
                </tr>
            </table>
        </td>
        <td>
            <table>
                <tr> 
                    <td> 1 </td>
                    <td> 2 </td>
                    <td> 3 </td>
                    <td> 4 </td>
                </tr>
                <tr> 
                    <td> Q </td>
                    <td> W </td>
                    <td> E </td>
                    <td> R </td>
                </tr>
                <tr> 
                    <td> A </td>
                    <td> S </td>
                    <td> D </td>
                    <td> F </td>
                </tr>
                <tr> 
                    <td> Z </td>
                    <td> X </td>
                    <td> C </td>
                    <td> V </td>
                </tr>
            </table>
        </td>
    </tr>
</table>