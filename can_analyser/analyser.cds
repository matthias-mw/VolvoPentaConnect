{
    "connections": [
        {
            "in_id": "{73cfd252-d29c-4da0-8a34-bfae1acfd761}",
            "in_index": 0,
            "out_id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
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
                "x": 410,
                "y": 138
            }
        },
        {
            "id": "{c227ca77-9677-489a-8fc9-e3082aa17f2b}",
            "model": {
                "backend": "socketcan",
                "caption": "CanDevice #1",
                "configuration": "",
                "interface": "can0",
                "name": "CanDevice"
            },
            "position": {
                "x": 141,
                "y": 133
            }
        }
    ]
}
