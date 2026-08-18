use std::ffi::c_int;

#[repr(C)]
#[derive(Debug, Copy, Clone, Eq, PartialEq)]
pub enum Sc2OptStatus {
    Ok = 0,
    InvalidArgument = 1,
    OutputTooSmall = 2,
    InternalError = 3,
}

#[link(name = "sc2opt_c")]
unsafe extern "C" {
    pub fn sc2opt_c_api_version_major() -> u32;
    pub fn sc2opt_c_api_version_minor() -> u32;
    pub fn sc2opt_c_registry_unit_count() -> usize;
    pub fn sc2opt_c_distance_squared_into(
        xy: *const f32,
        xy_count: usize,
        anchor_x: f32,
        anchor_y: f32,
        output: *mut f32,
        output_count: usize,
        written: *mut usize,
    ) -> c_int;
    pub fn sc2opt_c_within_radius_mask_into(
        xy: *const f32,
        xy_count: usize,
        anchor_x: f32,
        anchor_y: f32,
        radius: f32,
        output: *mut u8,
        output_count: usize,
        written: *mut usize,
    ) -> c_int;
}
