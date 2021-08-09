# SolidEarthFlexing
_created by: MPZinke_ <br/>
C++ translated version of solid.f by Dennis Milbert https://geodesyworld.github.io/SOFTS/solid.htm

## About

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

### `solid.f`

#### Subroutine/Function Calls

```txt
MAIN
|  geoxyz
|  civmjd
|  mjdciv
|  setjd0
|  sunxyz
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
|  |  getghar
|  |  rot3
|  moonxyz
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
|  |  rot1
|  |  getghar
|  |  rot3
|  detide
|  |  utc2ttt
|  |  |  utc2tai
|  |  |  tai2tt
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
|  |  step2lon
|  rge
|  mjdciv
```

#### Subroutine/Functions

```txt
1. detide [Subroutine]
	- Params: xsta, mjd, fmjd, xsun, xmon, dxtide, lflag
	- Altered Params: lflag, dxtide
2. st1l1 [Subroutine]
	- Params: xsta, xsun, xmon, fac2sun, fac2mon, xcorsta
	- Altered Params: xcorsta
3. step2diu [Subroutine]
	- Params: xsta, fhr, t, xcorsta
	- Altered Params: xcorsta
4. step2lon [Subroutine]
	- Params: xsta, fhr, t, xcorsta
	- Altered Params: xcorsta
5. st1idiu [Subroutine]
	- Params: xsta, xsun, xmon, fac2sun, fac2mon, xcorsta
	- Altered Params: xcorsta
6. st1isem [Subroutine]
	- Params: xsta, xsun, xmon, fac2sun, fac2mon, xcorsta
	- Altered Params: xcorsta
7. sprod [Subroutine]
	- Params: x, y, scal, r1, r2
	- Altered Params: r1, r2, scal
8. enorm8 [Function]
	- Params: a
9. zero_vec8 [Subroutine]
	- Params: v
	- Altered Params: v
10. moonxyz [Subroutine]
	- Params: mjd, fmjd, rm, lflag
	- Altered Params: lflag
11. getghar [Subroutine]
	- Params: mjd, fmjd, ghar
	- Altered Params: ghar
12. sunxyz [Subroutine]
	- Params: mjd, fmjd, rs, lflag
	- Altered Params: lflag
13. lhsaaz [Subroutine]
	- Params: u, v, w, ra, az, va
	- Altered Params: ra, az, va
14. geoxyz [Subroutine]
	- Params: gla, glo, eht, x, y, z
	- Altered Params: x, y, z
15. rge [Subroutine]
	- Params: gla, glo, u, v, w, x, y, z
	- Altered Params: u, v, w
16. rot1 [Subroutine]
	- Params: theta, x, y, z, u, v, w
	- Altered Params: u, v, w
17. rot3 [Subroutine]
	- Params: theta, x, y, z, u, v, w
	- Altered Params: u, v, w
18. setjd0 [Subroutine]
	- Params: iyr, imo, idy
	- Altered Params:
19. civjts [Subroutine]
	- Params: iyr, imo, idy, ihr, imn, sec, tsec
	- Altered Params: tsec
20. jtsciv [Subroutine]
	- Params: tsec, iyr, imo, idy, ihr, imn, sec
	- Altered Params: idy, imo, iyr, ihr, imn, sec
21. civmjd [Subroutine]
	- Params: iyr, imo, idy, ihr, imn, sec, mjd, fmjd
	- Altered Params: mjd, fmjd
22. mjdciv [Subroutine]
	- Params: mjd, fmjd, iyr, imo, idy, ihr, imn, sec
	- Altered Params: idy, imo, iyr, ihr, imn, sec
23. utc2ttt [Function]
	- Params: tutc
24. gps2ttt [Function]
	- Params: tgps
25. utc2tai [Function]
	- Params: tutc
26. getutcmtai [Function]
	- Params: tsec
27. tai2tt [Function]
	- Params: ttai
28. gps2tai [Function]
	- Params: tgps
```
