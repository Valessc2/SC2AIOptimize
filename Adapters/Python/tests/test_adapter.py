from array import array

import _sc2opt


def main() -> None:
    xy = array("f", [0.0, 0.0, 3.0, 4.0, 6.0, 8.0])
    distances = array("f", [0.0, 0.0, 0.0])

    written = _sc2opt.distance_squared_into(xy, distances, 0.0, 0.0)
    assert written == 3
    assert list(distances) == [0.0, 25.0, 100.0]

    mask = bytearray(3)
    written = _sc2opt.within_radius_mask_into(xy, mask, 0.0, 0.0, 5.0)
    assert written == 3
    assert list(mask) == [1, 1, 0]

    build, data_version = _sc2opt.registry_identity()
    assert build == "75689"
    assert data_version == "B89B5D6FA7CBF6452E721311BFBC6CB2"
    assert _sc2opt.registry_unit_count() == 245

    try:
        _sc2opt.distance_squared_into(array("f", [1.0]), distances, 0.0, 0.0)
    except ValueError:
        pass
    else:
        raise AssertionError("odd coordinate input must fail closed")

    try:
        _sc2opt.within_radius_mask_into(xy, mask, 0.0, 0.0, -1.0)
    except ValueError:
        pass
    else:
        raise AssertionError("negative radius must fail closed")


if __name__ == "__main__":
    main()
