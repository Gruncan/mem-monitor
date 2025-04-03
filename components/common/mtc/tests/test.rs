use mtc::MtcObjectFfi;

#[test]
fn test_decode() {
    let mut mtc_object = MtcObjectFfi::new();
    mtc_object.decode("/home/duncan/Development/Uni/Thesis/Logs/chrome9gavailable/chrome9gavailablesystem.mtc");

    println!("Version: {}\nSize: {}\n", mtc_object.get_version(), mtc_object.get_size());
}


