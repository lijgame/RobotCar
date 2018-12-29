import React from 'react';
import { Animated, Image, PanResponder, StyleSheet, Text, View, Button } from 'react-native';
const Radius = 150;
export default class App extends React.Component {
  constructor(props) {
    super(props);

    this.state = { pan: new Animated.ValueXY(), offsetX: '0', offsetY: '0' };
  }
  render() {
    let { pan } = this.state;

    let [translateX, translateY] = [pan.x, pan.y];
    // console.log('X: ' + translateX._value + ' Y: ' + translateY._value);

    let imageStyle = {
      transform: [{ translateX }, { translateY }],
      alignItems: 'center',
      justifyContent: 'center',
    };
    return (
      <View style={styles.container}>
        <View style={styles.circle}>
          <Animated.View style={imageStyle} {...this._panResponder.panHandlers}>
            <Image source={require('./assets/button.png')}>
            </Image>
          </Animated.View>
        </View>
        <Text>X: {translateX._value.toFixed(0)} Y:{translateY._value.toFixed(0)}</Text>
        <View style={styles.buttonContainer}>
          <Button
            // style={styles.button}
            onPress={this.AutoButtonClicked}
            title="Auto"
          />
        </View>
      </View>);
  }
  AutoButtonClicked() {
    alert('Auto button is clicked');
  }
  componentWillMount() {
    this.state.pan.setValue({ x: 0, y: 0 });

    // let mover =
    //     Animated.event([null, {dx: this.state.pan.x, dy: this.state.pan.y}]);

    this._panResponder = PanResponder.create({
      onMoveShouldSetResponderCapture: () => true,
      onMoveShouldSetPanResponderCapture: () => true,
      onPanResponderGrant: (e, s) => {
        this.state.pan.setOffset(
          { x: this.state.pan.x._value, y: this.state.pan.y._value });
        this.state.pan.setValue({ x: 0, y: 0 });
      },
      onPanResponderMove: (e, s) => {
        let ox = s.dx, oy = s.dy;
        let r = Math.sqrt(ox * ox + oy * oy);
        if (r > Radius) {
          let scale = Radius / r;
          ox *= scale;
          oy *= scale;

        }
        this.setState({ offsetX: ox, offsetY: oy });
        this.state.pan.setValue({ x: ox, y: oy });

        return Animated.event(
          [e, { dx: ox, dy: oy }]);

        // return mover(e, s);
      },
      onPanResponderRelease: (e, s) => {
        this.state.pan.setValue({ x: 0, y: 0 });
        this.state.pan.flattenOffset();
        this.setState({ offsetX: 0, offsetY: 0 });
      }
    })
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 0,
    flexDirection: 'column',
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
    marginVertical: 50,
  },
  buttonContainer: {
    flex: 0,
    flexDirection: 'row',
    backgroundColor: '#fff',
    // alignItems: 'center',
    justifyContent: 'space-evenly',
    marginVertical: 50,
    padding: 80
  },
  circle: {
    width: Radius * 2,
    height: Radius * 2,
    borderRadius: Radius,
    backgroundColor: 'gray',
    alignItems: 'center',
    justifyContent: 'center',

    // marginVertical: 50,

  },
  button: {
    width: 80,
    height: 30,
  }
});
