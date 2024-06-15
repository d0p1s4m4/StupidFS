# StupidFS

```mermaid
block-beta
  block
          a["boot block"]
          b["super block"]
          c["inodes"]
          d["inodes"]
          f["data"]
          g["data"]
          h["data"]
  end
```

```mermaid
block-beta
 columns 1
  block:a
    ab0["block 0"]
    ab1["block 1"]
    ab2["block 2"]
    ab3("...")
    ab4["block 99"]
  end

  block:b
    bb0["block 0"]
    bb1["block 1"]
    bb2["block 2"]
    bb3("...")
    bb4["block 99"]
  end

  block:c
    cb0["block 0"]
    cb1["block 1"]
    cb2["block 2"]
    cb3("...")
    cb4["block 99"]
  end

  ab0 --> bb0
  bb0 --> cb0
```

## License

<img src="https://opensource.org/wp-content/themes/osi/assets/img/osi-badge-light.svg" align="right" height="128px" alt="OSI Approved License">

StupidFS is dual-licensed under both CeCiLL-B and BSD-3-Clause licenses.

The full text of the CeCiLL-B license can be accessed via [this link](https://cecill.info/licences/Licence_CeCILL-B_V1-en.html) and is also included in [the license file](LICENSE) of this software package.

The full text of the BSD-3-Clause license can be accessed via [this link](https://opensource.org/licenses/BSD-3-Clause) and is also included in [the license file](LICENSE.BSD3) of this software package.
