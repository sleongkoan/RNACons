package filters;
import java.util.List;

public interface Filter<T> {

    List<T> filter(List<T> inputList);
}
