use mtc::{MtcObjectFfi, MtcResult};

#[test]
fn test_decode() -> MtcResult<()>{
    let mut mtc_object = MtcObjectFfi::new()?;

    mtc_object.decode("/home/duncan/Development/Uni/Thesis/Logs/chrome9gavailable/chrome9gavailablesystem.mtc")?;

    println!("Version: {}\nSize: {}\n", mtc_object.get_version(), mtc_object.get_size());

    let times = mtc_object.get_times()?;

    assert_eq!(times.len(), mtc_object.get_size() as usize);

    Ok(())
}


