mod core;
use server::*;

fn main() {
    let server = init_server();
    if let Ok(mut server) = server {
        main_loop(&mut server);
    }

   // println!("Hello, world!");
   // test_server();
}
