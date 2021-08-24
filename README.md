# SolidEarthFlexing
_created by: MPZinke_ <br/>
C++ translated version of [solid.f by Dennis Milbert](https://geodesyworld.github.io/SOFTS/solid.htm)

## About

As the Earth rotates the sun & moon pull on the tides and the crust. The force from these pulls and changes in ocean changes the shape of the Earth. `solid.f` by Dennis Milbert calculates the change in meters of the planet. However, because it is Fortran code, maintaining it is becoming increasingly difficult, as it is an older language. This program's aim is to allow for maintainability and object oriented practices (for code modification). Additionally, variable names are (hopefully) more spelled out, to allow for convenience in understanding what is going on. For parts of the code that I am able to find resources for, but are not well commented, I have added additional comments & resources.

## Contents

1. `./Headers/`—Contains header files for C++ translated code.
2. `./SolidEarthFlexing/`—Contains Fortran code & documentation by original author.
3. `./Source`—Contains source files for C++ translated code.
4. `./.gitignore`—Git Ignore file so that I don't upload a bunch of junk (you're welcome).
5. `./README.md`—This file.
6. `./Makefile`—Makefile to compile all C++ code.

---

## Use

### Compiling

```bash
make
```

### Running

```bash
./SolidEarthFlexing
```
Or with input file
```bash
./SolidEarthFlexing < input.txt	
```

### Testing
```bash
git checkout Testing
make test
./Testing/SolidEarthFlexing < ./Testing/TestInput.txt
```

## Details



---

## Appendix

### Type Promotion
Type conversion is an important consideration when translating between these two languages, because some precision is intentionally lost in the Fortran program. To account for this, the rules of both languages are detailed below.

#### Fortran
Fortran (mixed mode) automatically promotes a value to a type of the operand with the higher rank. EG. `int` * `double` is equivalent to `double` * `double`.<br/>
This is supported by [this reference](https://docs.oracle.com/cd/E19957-01/805-4939/z400073a2265/index.html).
> Double precision operators apply to only double precision operands, and any operand of lower precision is promoted to double precision.

#### C++
C++ promotes based on 

---

### `solid.f`

#### Subroutine/Function Calls

```txt
MAIN
|  *datan
|  geoxyz
|  |  *dsin
|  |  *dcos
|  |  *dsqrt
|  |  *dcos
|  |  *dsin
|  civmjd
|  mjdciv
|  setjd0
|  sunxyz
|  |  *dsin
|  |  *dcos
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  |  getutcmtai
|  |  |  tai2tt
|  |  *dcos
|  |  *dcos
|  |  *dsin
|  |  *dsin
|  |  *dsin
|  |  *dcos
|  |  getghar
|  |  rot3
|  |  |  *dsin
|  |  |  *dcos
|  moonxyz
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  |  getutcmtai
|  |  |  tai2tt
|  |  *dsin
|  |  *dcos
|  |  *dsin
|  |  *dcos
|  |  rot1
|  |  |  *dsin
|  |  |  *dcos
|  |  getghar
|  |  rot3
|  |  |  *dsin
|  |  |  *dcos
|  detide
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  |  getutcmtai
|  |  |  tai2tt
|  |  *int
|  |  sprod
|  |  sprod
|  |  zero_vec8
|  |  st1idiu
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  st1isem
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  st1l1
|  |  |  enorm8
|  |  |  enorm8
|  |  |  enorm8
|  |  step2diu
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *sin
|  |  |  *cos
|  |  |  *sin
|  |  |  *cos
|  |  |  *cos
|  |  |  *sin
|  |  step2lon
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *dmod
|  |  |  *cos
|  |  |  *sin
|  |  |  *cos
|  |  |  *sin
|  rge
|  |  *dsin
|  |  *dcos
|  |  *dsin
|  |  *dcos
|  mjdciv
|  *idnint
```

#### Subroutine/Functions

1. `detide` [Subroutine]
    - Params: `xsta`, `mjd`, `fmjd`, `xsun`, `xmon`, `dxtide`, `lflag`
    - Altered Params: `lflag`, `dxtide`
2. `st1l1` [Subroutine]
    - Params: `xsta`, `xsun`, `xmon`, `fac2sun`, `fac2mon`, `xcorsta`
    - Altered Params: `xcorsta`
3. `step2diu` [Subroutine]
    - Params: `xsta`, `fhr`, `t`, `xcorsta`
    - Altered Params: `xcorsta`
4. `step2lon` [Subroutine]
    - Params: `xsta`, `fhr`, `t`, `xcorsta`
    - Altered Params: `xcorsta`
5. `st1idiu` [Subroutine]
    - Params: `xsta`, `xsun`, `xmon`, `fac2sun`, `fac2mon`, `xcorsta`
    - Altered Params: `xcorsta`
6. `st1isem` [Subroutine]
    - Params: `xsta`, `xsun`, `xmon`, `fac2sun`, `fac2mon`, `xcorsta`
    - Altered Params: `xcorsta`
7. `sprod` [Subroutine]
    - Params: `x`, `y`, `scal`, `r1`, `r2`
    - Altered Params: `r1`, `r2`, `scal`
8. `enorm8` [Function]
    - Params: `a`
9. `zero_vec8` [Subroutine]
    - Params: `v`
    - Altered Params: `v`
10. `moonxyz` [Subroutine]
    - Params: `mjd`, `fmjd`, `rm`, `lflag`
    - Altered Params: `lflag`
11. `getghar` [Subroutine]
    - Params: `mjd`, `fmjd`, `ghar`
    - Altered Params: `ghar`
12. `sunxyz` [Subroutine]
    - Params: `mjd`, `fmjd`, `rs`, `lflag`
    - Altered Params: `lflag`
13. `lhsaaz` [Subroutine]
    - Params: `u`, `v`, `w`, `ra`, `az`, `va`
    - Altered Params: `ra`, `az`, `va`
14. `geoxyz` [Subroutine]
    - Params: `gla`, `glo`, `eht`, `x`, `y`, `z`
    - Altered Params: `x`, `y`, `z`
15. `rge` [Subroutine]
    - Params: `gla`, `glo`, `u`, `v`, `w`, `x`, `y`, `z`
    - Altered Params: `u`, `v`, `w`
16. `rot1` [Subroutine]
    - Params: `theta`, `x`, `y`, `z`, `u`, `v`, `w`
    - Altered Params: `u`, `v`, `w`
17. `rot3` [Subroutine]
    - Params: `theta`, `x`, `y`, `z`, `u`, `v`, `w`
    - Altered Params: `u`, `v`, `w`
18. `setjd0` [Subroutine]
    - Params: `iyr`, `imo`, `idy`
    - Altered Params:
19. `civjts` [Subroutine]
    - Params: `iyr`, `imo`, `idy`, `ihr`, `imn`, `sec`, `tsec`
    - Altered Params: `tsec`
20. `jtsciv` [Subroutine]
    - Params: `tsec`, `iyr`, `imo`, `idy`, `ihr`, `imn`, `sec`
    - Altered Params: `idy`, `imo`, `iyr`, `ihr`, `imn`, `sec`
21. `civmjd` [Subroutine]
    - Params: `iyr`, `imo`, `idy`, `ihr`, `imn`, `sec`, `mjd`, `fmjd`
    - Altered Params: `mjd`, `fmjd`
22. `mjdciv` [Subroutine]
    - Params: `mjd`, `fmjd`, `iyr`, `imo`, `idy`, `ihr`, `imn`, `sec`
    - Altered Params: `idy`, `imo`, `iyr`, `ihr`, `imn`, `sec`
23. `utc2ttt` [Function]
    - Params: `tutc`
24. `gps2ttt` [Function]
    - Params: `tgps`
25. `utc2tai` [Function]
    - Params: `tutc`
26. `getutcmtai` [Function]
    - Params: `tsec`
27. `tai2tt` [Function]
    - Params: `ttai`
28. `gps2tai` [Function]
    - Params: `tgps`

---
