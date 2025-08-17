# SSFIX (Simple Financial Information eXchange Protocol)

⚠️  Work in progress !!

```
8=<Protocol version:char*> 9=<body length:int> 35=<Message type:char> ... 10=<checksum:int>
```

# Available Messge type

📚 If `Message type` is an uppercase letter, the message contain a list of elements

- `C`: candle
- `O`: order
- `X`: exit
- `R`: reset
- `S`: state
- `E`: error

```yaml
CandleMessage:
  8:   
    description: Protocol version
    type: string
    example: "SFIX.1"
  9:   
    description: Body length
    type: integer
    example: 146
  12:  
    description: Timestamp (in microseconds or another format)
    type: string
    example: "100000"
  35:  
    description: Message type
    type: string
    fixed_value: "C"  # C for Candle
  61:
    description: Date (YYYY-MM-DD)
    type: string
    example: "2024-01-01"
  62:
    description: Time (HH:MM:SS)
    type: string
    example: "08:00:00"
  63:
    description: Open price
    type: decimal
    example: "10.5000000000"
  64:
    description: High price
    type: decimal
    example: "10.5000000000"
  65:
    description: Low price
    type: decimal
    example: "10.5000000000"
  66:
    description: Close price
    type: decimal
    example: "10.5000000000"
  67:
    description: Up volume
    type: integer
    example: 1000
  69:
    description: Down volume
    type: integer
    example: 1000
  70:
    description: Total volume
    type: integer
    example: 1000
  71:
    description: Up ticks
    type: integer
    example: 10
  72:
    description: Down ticks
    type: integer
    example: 10
  73:
    description: Total ticks
    type: integer
    example: 10
```

```yaml
OrderMessage:
  8:   
    description: Protocol version
    type: string
    example: "SFIX.1"
  9:   
    description: Body length
    type: integer
    example: 73
  11:
    description: Client order ID
    type: string
    example: "aaa"
  12:
    description: Timestamp (e.g., microseconds since epoch or exchange time)
    type: string
    example: "100000"
  35:
    description: Message type
    type: string
    fixed_value: "O"  
  38:
    description: Quantity
    type: decimal
    example: "10.0000000000"
  44:
    description: Price
    type: decimal
    example: "14.4500000000"
  45:
    description: Order type
    type: string
    enum: ["MARKET", "LIMIT", "STOP LOSS"]
    example: "MARKET"
  46:
    description: Order status
    type: string
    enum: ["PENDING", "EXECUTED", "REJECTED", "CANCELLED"]
    example: "CANCELLED"
  54:
    description: Order side
    type: string
    enum: ["BUY", "SELL", "NONE"]
    example: "NONE"
  55:
    description: Symbol
    type: string
    example: "BTC"
```
