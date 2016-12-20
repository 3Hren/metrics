TEST(_, _) {
    registry_t registry;
    auto metrics = registry.select(
        and_t{
            eq_t{"service", "node"},
            eq_t{"type", "counter"}
        }
    );
}

select * where service=node =>

"/node/pool/size" = 4
"/node/queue/depth" = 30
"/node/queue/limit" = 100

dump(json_t) =>

{
    "node": {
        "queue": {
            "depth": 30,
            "limit": 100
        },
        "pool": {
            "size": 4
        }
    }
}
