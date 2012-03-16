package main
import "fmt"
type Err string
type In interface {
}
func main() {
    var a In = "dfa"
    x, ok := a.(Err)
    fmt.Println(x, ok)
}

