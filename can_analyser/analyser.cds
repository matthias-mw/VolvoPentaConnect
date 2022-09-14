{
    "connections": [
        {
            "in_id": "{d8a036fb-7dba-461b-8b1a-cdecaaa838f4}",
            "in_index": 0,
            "out_id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "out_index": 0
        },
        {
            "in_id": "{73cfd252-d29c-4da0-8a34-bfae1acfd761}",
            "in_index": 0,
            "out_id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "out_index": 0
        },
        {
            "in_id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "in_index": 0,
            "out_id": "{489628b4-b294-4ec3-ad96-ec29e182cac8}",
            "out_index": 0
        },
        {
            "in_id": "{489628b4-b294-4ec3-ad96-ec29e182cac8}",
            "in_index": 0,
            "out_id": "{3c353a42-f2e4-4891-acd0-9359326f1e8e}",
            "out_index": 0
        },
        {
            "in_id": "{fb625936-b96f-40d6-9527-878d6e4633ee}",
            "in_index": 0,
            "out_id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "out_index": 0
        },
        {
            "in_id": "{6924d397-7848-4fc8-96dc-9241eec1163c}",
            "in_index": 0,
            "out_id": "{fb625936-b96f-40d6-9527-878d6e4633ee}",
            "out_index": 0
        }
    ],
    "nodes": [
        {
            "id": "{73cfd252-d29c-4da0-8a34-bfae1acfd761}",
            "model": {
                "caption": "CanRawView #2",
                "name": "CanRawView",
                "scrolling": false,
                "viewColumns": [
                    {
                        "name": "time",
                        "vIdx": 1
                    },
                    {
                        "name": "id",
                        "vIdx": 2
                    },
                    {
                        "name": "dir",
                        "vIdx": 3
                    },
                    {
                        "name": "len",
                        "vIdx": 4
                    },
                    {
                        "name": "data",
                        "vIdx": 5
                    }
                ]
            },
            "position": {
                "x": 414,
                "y": 142
            }
        },
        {
            "id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "model": {
                "backend": "socketcan",
                "caption": "Microchip CAN Analyzer",
                "configuration": "BitRateKey=250000",
                "interface": "can0",
                "name": "CanDevice"
            },
            "position": {
                "x": 141,
                "y": 133
            }
        },
        {
            "id": "{d8a036fb-7dba-461b-8b1a-cdecaaa838f4}",
            "model": {
                "bitrate [bps]": "500000",
                "caption": "CanLoad #3",
                "name": "CanLoad",
                "period [ms]": "1000"
            },
            "position": {
                "x": 413,
                "y": 231
            }
        },
        {
            "id": "{20c04846-4832-450e-9f7e-a728dab68b93}",
            "model": {
                "caption": "CAN Database PGN",
                "color": "#FF55FF",
                "file": "/media/develop/ESP32/PlatformIO/VolvoPentaConnect/can_analyser/pgns_new2.dbc",
                "msgSettings": [
                ],
                "name": "CanSignalData"
            },
            "position": {
                "x": 675,
                "y": 33
            }
        },
        {
            "id": "{489628b4-b294-4ec3-ad96-ec29e182cac8}",
            "model": {
                "CAN database": null,
                "caption": "CanSignalEncoder #9",
                "color": "#FF55FF",
                "name": "CanSignalEncoder"
            },
            "position": {
                "x": 79,
                "y": 308
            }
        },
        {
            "id": "{3c353a42-f2e4-4891-acd0-9359326f1e8e}",
            "model": {
                "CAN database": "{20c04846-4832-450e-9f7e-a728dab68b93}",
                "caption": "CanSignalSender #8",
                "color": "#FF55FF",
                "name": "CanSignalSender",
                "rows": [
                    {
                        "id": "0x18ff1dfe",
                        "sig": "status",
                        "val": "1"
                    },
                    {
                        "id": "0x18ff0afe",
                        "sig": "industryCode",
                        "val": "9"
                    }
                ]
            },
            "position": {
                "x": 74,
                "y": 457
            }
        },
        {
            "id": "{fb625936-b96f-40d6-9527-878d6e4633ee}",
            "model": {
                "CAN database": "{20c04846-4832-450e-9f7e-a728dab68b93}",
                "caption": "CanSignalDecoder #6",
                "color": "#FF55FF",
                "name": "CanSignalDecoder"
            },
            "position": {
                "x": 398,
                "y": 337
            }
        },
        {
            "id": "{6924d397-7848-4fc8-96dc-9241eec1163c}",
            "model": {
                "caption": "CanSignalViewer #8",
                "name": "CanSignalViewer"
            },
            "position": {
                "x": 626,
                "y": 344
            }
        }
    ]
}
